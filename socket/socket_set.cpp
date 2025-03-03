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
#include <string.h>

#include "run_state.hpp"
#include "socket_set.hpp"
#include "util.hpp"

using namespace std;
using namespace adns;

socket_set::socket_set()
{
    m_epoll_fd = epoll_create(1);

    if (m_epoll_fd == -1)
    {
        THROW(socket_set_exception, "epoll_create failed", util::strerror(), errno);
    }

}

socket_set::~socket_set()
{
    ::close(m_epoll_fd);
}

void socket_set::add(const shared_ptr<adns::socket> &s)
{
    s->set_non_blocking();

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));

    ev.data.fd = s->get_fd(); 
    if (s->is_stream())
    {
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
    }
    else
    {
        ev.events = EPOLLIN | EPOLLET;
    }

    unique_lock<mutex> guard(m_lock);

    // make sure the socket is in the map first so that we don't get a race condition 
    m_socket_map[s->get_fd()] = s;

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, s->get_fd(), &ev) != 0)
    {
        // erase the socket from the map since we didn't manage to add it to the epoll set
        m_socket_map.erase(s->get_fd());

        THROW(socket_set_exception, "epoll_ctl failed", util::strerror(), errno);
    }

}

void socket_set::remove(const shared_ptr<adns::socket> &s)
{
    if (s->get_fd() == -1)
    {
        LOG(warning) << "attempt to remove non-connected socket from socket set";
    }
    else
    {
        unique_lock<mutex> guard(m_remove_list_lock);
        m_remove_list.push_back(s);
    }
}

socket_set::event socket_set::wait_one(uint timeout_ms)
{
    struct epoll_event events[1];

    unique_lock<mutex> guard(m_poll_lock);

    while (true)
    {
        if (run_state::o_state == run_state::shutdown_e)
        {
            THROW(socket_set_timeout_exception, "epoll_wait timeout");
        }

        int num_events = epoll_wait(m_epoll_fd, events, 1, timeout_ms);

        unique_lock<mutex> guard(m_lock);

        if (num_events == 0)
        {
            THROW(socket_set_timeout_exception, "epoll_wait timeout");
        }
        else if (num_events == -1)
        {
            if (errno == EINTR)
            {
                THROW(socket_set_timeout_exception, "epoll_wait timeout");
            }
            else
            {
                THROW(socket_set_exception, "epoll_wait failed", util::strerror(), errno);
            }
        }
        else
        {
            auto p = m_socket_map.find(events[0].data.fd);

            if (p == m_socket_map.end())
            {
                // the socket got removed by another thread so we don't care about it any more
                // timeout so the caller retries
                THROW(socket_set_timeout_exception, "epoll_wait timeout");
            }
            else
            {
                process_remove_list();

                if (events[0].events & EPOLLIN)
                {
                    return make_tuple(p->second, ready_read_e);
                }
                if (events[0].events & EPOLLOUT)
                {
                    return make_tuple(p->second, ready_write_e);
                }
                if (events[0].events & EPOLLRDHUP)
                {
                    return make_tuple(p->second, peer_hup_e);
                }
                else if (events[0].events & EPOLLERR)
                {
                    THROW(socket_set_exception, "epoll error event on socket", events[0].data.fd);
                }
                else
                {
                    LOG(warning) << "unexpected epoll event " << events[0].events << " on socket " << events[0].data.fd;
                }
            }
        }
    }
}

void socket_set::process_remove_list()
{
    unique_lock<mutex> guard(m_remove_list_lock);

    while (!m_remove_list.empty())
    {
        auto s = m_remove_list.front();
        auto p = m_socket_map.find(s->get_fd());

        if (p == m_socket_map.end())
        {
            //THROW(socket_set_exception, "attempt to remove socket from set that it wasn't in");
            LOG(warning) << "attempt to remove socket from set that it wasn't in";
        }
        else
        {
            struct epoll_event ev;
            memset(&ev, 0, sizeof(ev));

            ev.data.fd = s->get_fd(); 
            ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;

            m_socket_map.erase(p);

            if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, s->get_fd(), &ev) != 0)
            {
                THROW(socket_set_exception, "epoll_ctl failed", util::strerror(), errno);
            }
        }

        m_remove_list.pop_front();
    }
}
