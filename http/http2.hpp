// 
// Copyright 2025 Andrew Haisley
// 
// This program is free software: you can redistribute it and/or modify it under the terms 
// of the GNU General Public License as published by the Free Software Foundation, either 
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with this program. 
// If not, see https://www.gnu.org/licenses/.
// 
#pragma once

#include <memory>
#include <string>

#include <nghttp2/nghttp2.h>

#include "types.hpp"
#include "exception.hpp"
#include "tcp_socket.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http.hpp"
#include "http_impl.hpp"

EXCEPTION_CLASS(http2_exception, http_exception)
EXCEPTION_CLASS(http2_invalid_stream_exception, http2_exception)

#define HTTP2_MAX_HEADERS 100

namespace adns
{
    class http2 : public http_impl
    {
    public:

        /**
         * new HTTP session. Note that this is stateful and designed to live for the length
         * of the HTTP connection. HTTP 1.1 and HTTP 2 are handled with the protocol choice
         * coming from the socket and negotiated at connection time.
         */
        http2(http::usage_t u, tcp_socket &s, uint timeout);

        /**
         * cleanup
         */
        virtual ~http2();

        /**
         * Read a request from a stream socket. The payload (if any) is assumed
         * to be in JSON format. HTTP 1.1 / HTTP 2 is discovered from the socket.
         */
        std::shared_ptr<http_request> from_wire();

        /**
         * Send a response to a stream socket. 
         */
        void to_wire(http_response &res);

    private:

        // what the session is being used for
        http::usage_t m_usage;

        // socket for the session
        tcp_socket &m_socket;

        // TCP read/write timeout in ms
        uint m_timeout;

        // nghttp2 session pointer - allocated in constructor
        nghttp2_session *m_session;

        // maximum payload size we accept
        size_t m_max_payload_size;

        // payload contents by stream
        std::map<size_t, octet*> m_payload;

        // payload number of bytes read per stream
        std::map<size_t, size_t> m_payload_bytes_read;
        
        // parts of the request collected incrementally through nghttp2 callback functions
        std::map<size_t, std::unordered_map<std::string, std::string>> m_headers;

        std::map<size_t, std::string> m_raw_method;
        std::map<size_t, size_t>      m_content_length;
        std::map<size_t, std::string> m_path;

        // request constructed by on_frame_recv_callback
        std::shared_ptr<http_request> m_request;

        // callback which then calls the relevant member function when a header field is received
        static int on_header_callback(
            nghttp2_session     *session, 
            const nghttp2_frame *frame, 
            const uint8_t       *name, 
            size_t              namelen, 
            const uint8_t       *value, 
            size_t              valuelen,
            uint8_t             flags, 
            void                *http2_instance);

        // callback which then calls the relevant member function when some data is received
        static int on_data_chunk_callback(
            nghttp2_session *session, 
            uint8_t         flags, 
            int32_t         stream_id, 
            const uint8_t   *data, 
            size_t          len, 
            void            *http2_instance);

        // callback which then calls the relevant member function when a frame has been fully received
        static int on_frame_recv_callback(
            nghttp2_session     *session, 
            const nghttp2_frame *frame, 
            void                *http2_instance);

        // callback for getting the data to be sent in a response. Note that user_data can't be set to
        // anything due to the limited nghttp2 data provider interface
        static nghttp2_ssize read_data_callback(
                nghttp2_session     *session,
                int32_t             stream_id, 
                uint8_t             *buf,
                size_t              length, 
                uint32_t            *data_flags,
                nghttp2_data_source *source,
                void                *unused);

        // send data back to the client - calls the relevant member function
        static nghttp2_ssize send_callback(
                nghttp2_session *session,
                const uint8_t   *data, 
                size_t          length,
                int             flags, 
                void            *http2_instance);

        int on_header_callback(
            nghttp2_session     *session, 
            const nghttp2_frame *frame, 
            const uint8_t       *name, 
            size_t              namelen, 
            const uint8_t       *value, 
            size_t              valuelen,
            uint8_t             flags);

        int on_data_chunk_callback(
            nghttp2_session *session, 
            uint8_t         flags, 
            int32_t         stream_id, 
            const uint8_t   *data, 
            size_t          len);

        int on_frame_recv_callback(nghttp2_session *session, const nghttp2_frame *frame);

        nghttp2_ssize send_callback(
            nghttp2_session *session, 
            const uint8_t   *data, 
            size_t          length, 
            int             flags);
    };
}
