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

#include <sys/epoll.h>
#include <mutex>
#include <list>
#include <tuple>

#include "types.hpp"
#include "socket.hpp"
#include "exception.hpp"

#include <vector>
#include <unordered_map>

EXCEPTION_CLASS(socket_set_exception, exception)
EXCEPTION_CLASS(socket_set_timeout_exception, socket_set_exception)

#define SOCKET_SET_MAX_EVENTS 100

namespace adns
{
    /**
     * holds a set of sockets for later use in epoll and related system calls
     */
    class socket_set
    {
    public:

        typedef enum
        {
            ready_read_e,
            ready_write_e,
            peer_hup_e
        }
        event_type_t;

        typedef std::tuple<std::shared_ptr<socket>, event_type_t> event;

        /**
         * new empty set
         */
        socket_set();

        /**
         * destructor - doesn't delete any sockets in the set at the time of calling
         */
        virtual ~socket_set();

        /**
         * add a single socket
         */
        void add(const std::shared_ptr<socket> &s);

        /**
         * remove a single socket
         */
        void remove(const std::shared_ptr<socket> &s);

        /**
         * wait for one of the sockets in the set to have IO activity pending or throw a timeout exception 
         */
        event wait_one(uint timeout_ms=2000);

    private:

        // lock for the poll call
        std::mutex m_poll_lock;

        // lock for the following member variable
        std::mutex m_lock;

        // map of FDs to sockets
        std::unordered_map<int, std::shared_ptr<socket>> m_socket_map;

        // lock for the following member variable
        std::mutex m_remove_list_lock;

        // list of sockets waiting to be removed from the set. The actual
        // removal is done as part of the wait_many method, this is to stop
        // FDs being re-used whilst the epoll call is in progress thus
        // messing up the socket mapping
        std::list<std::shared_ptr<socket>> m_remove_list;

        // single epoll file descriptor
        int m_epoll_fd;

        /**
         * go through the contents of m_remove_list deleting them from set
         */
        void process_remove_list();
    };
}
