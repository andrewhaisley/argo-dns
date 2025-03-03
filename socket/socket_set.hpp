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
