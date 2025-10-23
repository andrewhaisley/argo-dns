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
 
#include <arpa/inet.h>
#include <unistd.h>

#include "types.hpp"
#include "control_connection.hpp"
#include "thread_pool.hpp"
#include "run_state.hpp"
#include "http.hpp"
#include "api.hpp"
#include "address_list.hpp"
#include "json_exception.hpp"

using namespace std;
using namespace adns;
using namespace boost::log::trivial;

control_connection::control_connection(
                        server_config                   &config, 
                        thread_pool<control_connection> &pool) :
                            m_config(config),
                            m_pool(pool)
{
    m_connection_arrived = false;
    m_receive_thread.reset(new thread(&control_connection::run, this));
}

control_connection::~control_connection()
{
}

void control_connection::done()
{
    lock_guard<mutex> guard(m_lock);
    m_socket = nullptr;
    m_connection_arrived = false;
    m_pool.put(this);
}

void control_connection::wait_for_connection()
{
    unique_lock<mutex> guard(m_lock);

    while (!m_connection_arrived)
    {
        if (m_condition.wait_for(guard, chrono::seconds(2)) == cv_status::timeout)
        {
            THROW(control_connection_timeout_exception, "timeout");
        }
    }
}

void control_connection::run()
{
    while (true)
    {
        try
        {
            while (true)
            {
                try
                {
                    wait_for_connection();
                    break;
                }
                catch (control_connection_timeout_exception &e)
                {
                    if (run_state::o_state == run_state::shutdown_e)
                    {
                        return;
                    }
                }
            }

            ip_address addr = m_socket->get_remote_address().get_ip_address();

            // always allow connections from localhost as a fallback recovery mechanism
            if (!addr.is_localhost() && !m_config.control.control_address_list->match(addr))
            {
                done();
                continue;
            }

            handle_requests();
            done();

            if (run_state::o_state == run_state::shutdown_e)
            {
                return;
            }
        }
        catch (adns::exception &e)
        {
            if (run_state::o_state == run_state::shutdown_e)
            {
                return;
            }

            done();
        }
    }
}

void control_connection::serve(const shared_ptr<tcp_socket> &socket)
{
    lock_guard<mutex> guard(m_lock);
    m_connection_arrived = true;
    m_socket = socket;
    m_condition.notify_one();
}

void control_connection::join()
{
    m_receive_thread->join();
}

void control_connection::handle_requests()
{
    api api_instance;

    bool close = false;

    http h(http::api_e, *m_socket, m_config.control.client_connection_timeout_ms);

    while (!close)
    {
        uint32_t stream_id = 0;

        try
        {
            auto req = h.from_wire();
            close = req->close_connection();
            stream_id = req->get_stream_id();

            // the to_wire method is all or nothing, the only exceptions
            // that will be thrown will be due to IO stuff that we can't
            // handle here
            h.to_wire(*api_instance.handle_request(req));
        }
        catch (http_payload_too_large_exception &e)
        {
            http_response r(stream_id, http_response::entity_too_large_413, e);
            h.to_wire(r);
            return;
        }
        catch (http_header_line_too_long &e)
        {
            http_response r(stream_id, http_response::header_fields_too_large_431, e);
            h.to_wire(r);
            return;
        }
        catch (http_request_bad_format_exception &e)
        {
            http_response r(stream_id, http_response::bad_request_400, e);
            h.to_wire(r);
            return;
        }
        catch (api_version_exception &e)
        {
            http_response r(stream_id, http_response::not_implemented_501, e);
            h.to_wire(r);
            return;
        }
        catch (api_malformed_exception &e)
        {
            http_response r(stream_id, http_response::bad_request_400, e);
            h.to_wire(r);
            return;
        }
        catch (api_unsupported_exception &e)
        {
            http_response r(stream_id, http_response::not_implemented_501, e);
            h.to_wire(r);
            return;
        }
        catch (api_resource_not_found_exception &e)
        {
            http_response r(stream_id, http_response::not_found_404, e);
            h.to_wire(r);
            return;
        }
        catch (api_method_not_allowed_exception &e)
        {
            http_response r(stream_id, http_response::method_not_allowed_405, e);
            h.to_wire(r);
            return;
        }
        catch (api_method_not_implemented_exception &e)
        {
            http_response r(stream_id, http_response::not_implemented_501, e);
            h.to_wire(r);
            return;
        }
        catch (api_method_conflict_exception &e)
        {
            http_response r(stream_id, http_response::conflict_407, e);
            h.to_wire(r);
            return;
        }
        catch (adns::exception &e)
        {
            http_response r(stream_id, http_response::internal_server_error_500, e);
            h.to_wire(r);
            return;
        }
        catch (json_exception &e)
        {
            control_connection_json_exception je(__PRETTY_FUNCTION__, __FILE__, __LINE__, e.what());
            http_response r(stream_id, http_response::internal_server_error_500, je);
            h.to_wire(r);
            return;
        }
    }
}
