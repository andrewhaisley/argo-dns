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
 
#pragma once

#include "types.hpp"
#include "ip_address.hpp"
#include "dns_message.hpp"
#include "exception.hpp"
#include "udp_socket.hpp"
#include "tcp_client_socket.hpp"

EXCEPTION_CLASS(dns_serial_client_exception, exception)
EXCEPTION_CLASS(dns_serial_client_truncated_reply_exception, dns_serial_client_exception)

namespace adns
{
    /**
     * Client that only tries to talk to one resolver at a time.
     * Primary use case is for zone transfers. Secondary is for
     * any situation where there is a very good reason to believe 
     * the target resolver might be up and running.
     */
    class dns_serial_client
    {
    public:

        /** 
         * no default constructr
         */
        dns_serial_client() = delete;

        /**
         * create a client to talk to a server at the given address/port
         */
        dns_serial_client(const socket_address &addr);

        /**
         * destructor - close socket etc.
         */
        virtual ~dns_serial_client();

        /**
         * attempt to send a message and get a response by the defined mechanism
         */
        std::shared_ptr<dns_message> send(
                            dns_message &request,
                            bool        try_udp,
                            bool        try_tcp,
                            uint        num_retries,
                            uint        response_timeout);

    private:

        udp_socket        m_udp_socket;
        tcp_client_socket m_tcp_socket;
        socket_address m_remote_addr;

        /**
         * attempt to send a message and get a response via udp messages
         */
        std::shared_ptr<dns_message> send_by_udp(
                            dns_message &request,
                            uint        num_retries,
                            uint        response_timeout);

        /**
         * send a request down the wire - don't wait for a response, that happens in the caller
         */
        void send_request_by_tcp(dns_message &request);

        /**
         * read a single response via TCP
         */
        std::shared_ptr<dns_message> read_response_by_tcp(uint response_timeout);

        /**
         * attempt to send a message and get a response via a tcp connection
         */
        std::shared_ptr<dns_message> send_by_tcp(
                            dns_message &request,
                            uint        num_retries,
                            uint        response_timeout);

        /**
         * make a single attempt to contact a server via UDP
         */
        std::shared_ptr<dns_message> send_by_udp_once(dns_message &request, uint packet_size, uint response_timeout);

        /**
         * wait until a matching reply from the correct remote server arrives.
         * Throw exception on timeout or mangled reply with a matching id.
         */
        std::shared_ptr<dns_message> get_matching_response(uint msg_id, uint response_timeout);

        /**
         * make a single attempt to contact a server via TCP
         */
        std::shared_ptr<dns_message> send_by_tcp_once(dns_message &request, uint response_timeout);
    };
}
