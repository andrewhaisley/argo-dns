//
//  Copyright 2025 Andrew Haisley
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the “Software”), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
 
#include <string.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>    

#include "types.hpp"
#include "unparser.hpp"
#include "datetime.hpp"
#include "run_state.hpp"
#include "http2.hpp"
#include "http_request.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

int http2::on_header_callback(
            nghttp2_session     *session, 
            const nghttp2_frame *frame, 
            const uint8_t       *name, 
            size_t              namelen, 
            const uint8_t       *value, 
            size_t              valuelen,
            uint8_t             flags, 
            void                *http2_instance)
{
    http2 *o = reinterpret_cast<http2 *>(http2_instance);
    return o->on_header_callback(session, frame, name, namelen, value, valuelen, flags);
}

int http2::on_data_chunk_callback(
            nghttp2_session *session, 
            uint8_t         flags, 
            int32_t         stream_id, 
            const uint8_t   *data, 
            size_t          len, 
            void            *http2_instance)
{
    http2 *o = reinterpret_cast<http2 *>(http2_instance);
    return o->on_data_chunk_callback(session, flags, stream_id, data, len);
}

int http2::on_frame_recv_callback(
        nghttp2_session     *session, 
        const nghttp2_frame *frame, 
        void                *http2_instance)
{
    http2 *o = reinterpret_cast<http2 *>(http2_instance);
    return o->on_frame_recv_callback(session, frame);
}

ssize_t http2::send_callback(
        nghttp2_session *session,
        const uint8_t   *data, 
        size_t          length,
        int             flags, 
        void            *http2_instance)
{
    http2 *o = reinterpret_cast<http2 *>(http2_instance);
    return o->send_callback(session, data, length, flags);
}

int http2::on_header_callback(
            nghttp2_session     *session, 
            const nghttp2_frame *frame, 
            const uint8_t       *name, 
            size_t              namelen, 
            const uint8_t       *value, 
            size_t              valuelen,
            uint8_t             flags)
{
    string n((char *)name, namelen);
    string v((char *)value, valuelen);

    boost::algorithm::to_lower(n);

    auto stream_id = frame->hd.stream_id;

    if (m_headers.find(stream_id) == m_headers.end())
    {
        m_headers[stream_id] = unordered_map<string, string>();
        m_raw_method[stream_id] = "";
        m_content_length[stream_id] = 0;
        m_path[stream_id] = "";
        m_payload_bytes_read[stream_id] = 0;
        m_payload[stream_id] = new octet[m_max_payload_size];
    }

    m_headers[stream_id][n] = v;

    if (m_headers[stream_id].size() > HTTP2_MAX_HEADERS)
    {
        THROW(http_exception, "maximum number of HTTP2 headers exceeded");
    }

    if (n == "method")
    {
        m_raw_method[stream_id] = v;
    }
    else if (n == "content-length")
    {
        m_content_length[stream_id] = boost::lexical_cast<size_t>(v);

        if (m_content_length[stream_id] > m_max_payload_size)
        {
            THROW(http_exception, "maximum payload exceeded");
        }

    }
    else if (n == "path")
    {
        m_path[stream_id] = v;
    }

    return 0;
}

int http2::on_data_chunk_callback(
            nghttp2_session *session, 
            uint8_t         flags, 
            int32_t         stream_id, 
            const uint8_t   *data, 
            size_t          len)
{
    if (m_payload.find(stream_id) == m_payload.end())
    {
        THROW(http_exception, "invalid stream id");
    }

    if (m_payload_bytes_read[stream_id] + len > m_content_length[stream_id])
    {
        THROW(http_exception, "bytes in data frames greater than payload length header");
    }

    memcpy(m_payload[stream_id] + m_payload_bytes_read[stream_id], data, len);
    m_payload_bytes_read[stream_id] += len;

    return 0;
}

int http2::on_frame_recv_callback(nghttp2_session *session, const nghttp2_frame *frame)
{
    auto stream_id = frame->hd.stream_id;

    if (frame->hd.type == NGHTTP2_DATA)
    {
        auto m = m_headers[stream_id].find(":method");

        if (m == m_headers[stream_id].end())
        {
            THROW(http_exception, ":method header missing");
        }

        auto p = m_headers[stream_id].find(":path");

        if (p == m_headers[stream_id].end())
        {
            THROW(http_exception, ":path header missing");
        }

        m_request = make_shared<http_request>(
                stream_id,
                m_headers[stream_id],
                m->second,
                p->second,
                url(p->second),
                buffer(m_payload_bytes_read[stream_id], m_payload[stream_id]));
    }

    return 0;
}

http2::http2(http::usage_t u, tcp_socket &s, uint timeout) :
                            m_usage(u),
                            m_socket(s), 
                            m_timeout(timeout)
{
    if (m_usage == http::doh_e)
    {
        m_max_payload_size = 16384; // no point in having this any smaller than the max frame size
    }
    else
    {
        m_max_payload_size = 1024 * 1024; // bigger for API usage
    }

    nghttp2_session_callbacks *callbacks;
    nghttp2_session_callbacks_new(&callbacks);

    nghttp2_session_callbacks_set_send_callback(callbacks, send_callback);
    nghttp2_session_callbacks_set_on_header_callback(callbacks, on_header_callback);
    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, on_data_chunk_callback);
    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, on_frame_recv_callback);

    int r = nghttp2_session_server_new(&m_session, callbacks, this);

    nghttp2_session_callbacks_del(callbacks);

    if (r != 0) 
    {
        THROW(http_exception, "failed to create HTTP2 session");
    }

    r = nghttp2_submit_settings(m_session, NGHTTP2_FLAG_NONE, NULL, 0);

    if (r != 0) 
    {
        THROW(http_exception, "failed to submit settings");
    }
}

http2::~http2()
{
    (void)nghttp2_session_del(m_session);
    for (auto i : m_payload)
    {
        delete [] i.second;
    }
}

void make_nv(nghttp2_nv &nv, const string &name, const string &value)
{
    nv.flags = NGHTTP2_NV_FLAG_NONE;

    nv.name = (octet *)strdup(name.c_str());
    nv.namelen = name.size();
    nv.value = (octet *)strdup(value.c_str());
    nv.valuelen = value.size();
}

ssize_t http2::read_data_callback(
                nghttp2_session     *session,
                int32_t             stream_id, 
                uint8_t             *buf,
                size_t              length, 
                uint32_t            *data_flags,
                nghttp2_data_source *source,
                void                *user_data) 
{
    pair<size_t, buffer *> *br = (pair<size_t, buffer *> *)source->ptr;

    buffer *b = br->second; 
    size_t bytes_read_so_far = br->first;
    size_t bytes_available = 0;

    if (bytes_read_so_far + length > b->get_size())
    {
        bytes_available = b->get_size() - bytes_read_so_far;
    }
    else
    {
        bytes_available = length;
    }

    memcpy(buf, b->get_data() + bytes_read_so_far, bytes_available);

    br->first += bytes_available;

    if (br->first >= b->get_size())
    {
        *data_flags |= NGHTTP2_DATA_FLAG_EOF;
    }

    return bytes_available;
}

void http2::to_wire(http_response &res)
{
    string json_string;

    if (m_usage == http::api_e) 
    {
        if (res.get_json().get_instance_type() == json::object_e)
        {
            json_string << res.get_json();
        }
        add_headers(m_usage, res, json_string.size());
    }
    else
    {
        add_headers(m_usage, res, res.get_data().get_size());
    }

    nghttp2_nv hdrs[res.get_headers().size() + 1];

    int n = 0;

    make_nv(hdrs[n++], ":status", boost::lexical_cast<string>(res.get_status()));
    
    for (auto h : res.get_headers())
    {
        make_nv(hdrs[n++], h.first, h.second);
    }

    buffer b;

    if (m_usage == http::api_e)
    {
        b = buffer(json_string);
    }
    else
    {
        b = res.get_data();
    }

    pair<size_t, buffer *> br(0, &b);

    nghttp2_data_provider data_prd;
    data_prd.source.ptr = &br;
    data_prd.read_callback = read_data_callback;

    int r = nghttp2_submit_response(m_session, res.get_stream_id(), hdrs, n, &data_prd);
    
    if (r != 0)
    {
        THROW(http_exception, "nghttp2_submit_response2 error", nghttp2_strerror(r));
    }

    // clean up headers before the possibility of throwing anything
    for (int i = 0; i < n; i++)
    {
        free(hdrs[i].name);
        free(hdrs[i].value);
    }

    r = nghttp2_session_send(m_session);

    if (r != 0)
    {
        THROW(http_exception, "nghttp2_session_send error", nghttp2_strerror(r));
    }
}

shared_ptr<http_request> http2::from_wire()
{
    m_request = nullptr;

    while (true)
    {
        buffer b = m_socket.read(1, m_timeout);
        if (b.get_size() == 1)
        {
            int r = nghttp2_session_mem_recv(m_session, b.get_data(), 1);
        
            if (r < 0) 
            {
                THROW(http_exception, "nghttp2 session error", nghttp2_strerror(r));
            }
        
            if (m_request)
            {
                return m_request;
            }
        }
        else
        {
            THROW(http_exception, "no request received");
        }
    }
}

ssize_t http2::send_callback(
        nghttp2_session *session,
        const uint8_t   *data, 
        size_t          length,
        int             flags)
{
    m_socket.write(data, length, m_timeout);

    return length;
}
