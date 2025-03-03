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

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "types.hpp"
#include "exception.hpp"
#include "message_queue.hpp"
#include "run_state.hpp"

EXCEPTION_CLASS(queue_multiplexer_exception, exception)
EXCEPTION_CLASS(queue_multiplexer_timeout_exception, exception)

namespace adns
{
    /**
     * Multiplex/demultiplex for queues of messages
     */
    template <class MESSAGE> class queue_multiplexer final
    {
    public:

        /**
         * new instance - single in/out on the left, multiple in/out on the right
         */
        queue_multiplexer(message_queue<MESSAGE> &left_in, message_queue<MESSAGE> &left_out) : m_left_in(left_in), m_left_out(left_out)
        {
            m_num_channels = 0;
            m_demux_thread = std::make_shared<std::thread>(&queue_multiplexer::run_demux, this);
        }

        /**
         * add a channel
         */
        void add_channel(std::shared_ptr<message_queue<MESSAGE>> right_in, std::shared_ptr<message_queue<MESSAGE>> right_out)
        {
            m_right_in.push_back(right_in);
            m_right_out.push_back(right_out);
            m_mux_threads.push_back(std::make_shared<std::thread>(&queue_multiplexer::run_mux, this, right_in, m_num_channels));
            m_num_channels++;
        }

        /**
         * Destructor
         */
        virtual ~queue_multiplexer()
        {
        }

        /**
         * Wait for the threads to all finish what they're doing (which is never unless interrupted).
         */
        void join()
        {
            m_demux_thread->join();

            for (auto iter : m_mux_threads)
            {
                iter->join();
            }
        }

    private:

        int m_num_channels;

        message_queue<MESSAGE>                &m_left_in;
        message_queue<MESSAGE>                &m_left_out;

        std::vector<std::shared_ptr<message_queue<MESSAGE>>> m_right_in;
        std::vector<std::shared_ptr<message_queue<MESSAGE>>> m_right_out;

        std::shared_ptr<std::thread>              m_demux_thread;
        std::vector<std::shared_ptr<std::thread>> m_mux_threads;

        void run_demux()
        {
            while (true)
            {
                try
                {
                    try
                    {
                        auto m = m_left_in.dequeue();
                        int c = m->get_channel();
                        if (c < 0 || c > m_num_channels)    
                        {
                            THROW(queue_multiplexer_exception, "invalid channel", c);
                        }
                        else
                        {
                            m_right_out[c]->enqueue(m);
                        }
                    }
                    catch (message_queue_timeout_exception &e)
                    {
                        if (run_state::o_state == run_state::shutdown_e)
                        {
                            return;
                        }
                    }
                }
                catch (adns::exception &e) 
                {
                    e.log(::boost::log::trivial::error, "exception caught in queue_multiplexer::run_demux");
                }
            }
        }

        void run_mux(std::shared_ptr<message_queue<MESSAGE>> in, int channel)
        {
            while (true)
            {
                try
                {
                    try
                    {
                        auto m = in->dequeue();
                        m->set_channel(channel);
                        m_left_out.enqueue(m);
                    }
                    catch (message_queue_timeout_exception &e)
                    {
                        if (run_state::o_state == run_state::shutdown_e)
                        {
                            return;
                        }
                    }
                }
                catch (adns::exception &e) 
                {
                    e.log(::boost::log::trivial::error, "exception caught in queue_multiplexer::run_mux");
                }
            }
        }
    };
}
