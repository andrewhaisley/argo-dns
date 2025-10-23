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

#include <set>
#include <chrono>

#include "types.hpp"
#include "dns_message.hpp"
#include "udp_socket.hpp"
#include "tcp_client_socket.hpp"
#include "ip_address.hpp"
#include "client_config.hpp"

EXCEPTION_CLASS(parallel_client_exception, exception)

namespace adns
{
    class dns_parallel_client
    {
    public:

        /**
         * new client - set IP addresses etc below.
         */
        dns_parallel_client(const client_config &config);

        /** 
         * set the list of IP addresses to use
         */
        void set_ip_addresses(const std::set<ip_address> &ips);

        /** 
         * set a single addresses to use
         */
        void set_ip_address(const ip_address &ip);

        /**
         * destructor
         */
        virtual ~dns_parallel_client();

        /**
         * Perform a single query using the IP addresses supplied.
         */
        dns_message *query(dns_message &request);

        /**
         * close sockets when done with them.
         */
        void close_sockets();

        /**
         * reset the sockets so they get a new local port - for security reasons
         */
        void reset_sockets();

    private:

        // config
        const client_config &m_config;

        // UDP sockets to use (TCP ones are always transient)
        std::shared_ptr<udp_socket> m_ip4_socket;
        std::shared_ptr<udp_socket> m_ip6_socket;

        // total sent packets
        uint m_total_sent;
        uint m_received_matching_query;

        // time the query started
        std::chrono::steady_clock::time_point m_query_start_time;

        // full set of IPs that might be used 
        std::set<ip_address> m_all_ips;

        // actual list of IPs to be used selected from the full set
        std::set<ip_address> m_ips;
     
        /**
         * check timeout by looking at the total amount of time spent
         * so far
         */
        void check_timeout() const;

        /**
         * Query in parallel via UDP
         */
        dns_message *query_udp(dns_message &request);

        /**
         * Receive UDP responses and process them
         */
        dns_message *receive_udp(dns_message &request, std::set<unsigned short int> &ids, bool &format_error);

        /**
         * Query in serial via TCP using one address at a time.
         */
        dns_message *query_tcp(dns_message &request);

        /**
         * send a request down the wire - don't wait for a response, that happens in the caller
         */
        void send_request_by_tcp(tcp_socket &socket, dns_message &request);

        /**
         * read a single response via TCP
         */
        dns_message *read_response_by_tcp(tcp_socket &socket);

        /**
         * send a message via UDP to all IPs and record the IDs
         */
        bool send_udp(dns_message &request, bool use_edns, std::set<unsigned short int> &outbound_ids);

        /**
         * Extract a response that matches the query - IP, ID & Query Name - from
         * a list of UDP messages.
         */
        dns_message *extract_matching_response(
                        udp_socket::message          *messages[], 
                        int                          n, 
                        std::set<unsigned short int> ids,
                        const dns_name               &qname,
                        bool                         &format_error);
    };
}
