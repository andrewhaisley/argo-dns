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
#include <algorithm>
#include <thread>
#include <atomic>

#include "types.hpp"
#include "message_queue.hpp"
#include "queue_multiplexer.hpp"
#include "exception.hpp"
#include "run_state.hpp"
#include "monitor.hpp"

namespace adns
{
    /**
     * A pool of threads with an inbound and outbound message queue. 
     * Messages are enqueued using the enqueue() method and then passed at random to one 
     * of N PROCESSOR instances each of which runs in its own thread.
     * Results come back on a separate queue.
     */
    template <class MESSAGE, class PROCESSOR, class PROCESSOR_PARAMS> class handler_pool final
    {
    public:

        /**
         * Create a queue with the given maximum length and number_of_processor threads
         * to consume messages from it. When the max length is reached, the oldest
         * messages are dropped to make space for new ones.
         */
        handler_pool(std::string name, const PROCESSOR_PARAMS &params, int max_in_length, int max_out_length, int num_threads) :
            m_name(name),
            m_params(params),
            m_in_queue(new message_queue<MESSAGE>(m_name + " input queue", max_in_length)),
            m_out_queue(new message_queue<MESSAGE>(m_name + " output queue", max_out_length)),
            m_multiplexer(nullptr),
            m_available_threads_monitor_id(monitor::add_thing("handler pool", name, "available threads", num_threads)),
            m_free_threads(num_threads),
            m_num_threads(num_threads)
        {
            for (int i = 0; i < num_threads; i++)
            {
                auto p = new PROCESSOR(m_params, *m_out_queue);
                m_processors.push_back(std::shared_ptr<PROCESSOR>(p));
                m_threads.push_back(std::make_shared<std::thread>(&handler_pool::run, this, p));
            }

            (void)monitor::add_thing("handler pool", name, "maximum threads", num_threads);
        }

        /**
         * Destructor - delete any outstanding messages. The threads are cleaned up automatically
         * by destruction of the std::shared_ptr ojects in the thread list.
         */
        virtual ~handler_pool()
        {
            delete m_multiplexer;
            delete m_in_queue;
            delete m_out_queue;
        }

        /**
         * Wait for the threads to all finish what they're doing (which is never unless interrupted).
         */
        void join()
        {
            for (auto iter : m_threads)
            {
                iter->join();
            }

            for (auto iter : m_processors)
            {
                iter->join();
            }

            if (m_multiplexer)
            {
                m_multiplexer->join();
            }
        }

        /**
         * Add a message to the processing queue.
         * @return  true if there was space to enqueue the message, false otherwise
         */
        bool enqueue(MESSAGE m)
        {
            return m_in_queue->enqueue(m);
        }

        /**
         * Get the next message from the return queue.
         */
        MESSAGE dequeue()
        {
            return m_out_queue->dequeue();
        }

        /**
         * Get the multiplexer (created when first asked for)
         */
        queue_multiplexer<MESSAGE> *get_multiplexer()
        {
            if (m_multiplexer == nullptr)
            {
                m_multiplexer = new queue_multiplexer<MESSAGE>(*m_out_queue, *m_in_queue);
            }
            return m_multiplexer;
        }

    private:

        std::list<std::shared_ptr<std::thread>> m_threads;

        std::string m_name;
        PROCESSOR_PARAMS m_params;

        message_queue<MESSAGE> *m_in_queue;
        message_queue<MESSAGE> *m_out_queue;

        queue_multiplexer<MESSAGE> *m_multiplexer;

        std::list<std::shared_ptr<PROCESSOR>> m_processors;

        uint m_available_threads_monitor_id;
        std::atomic<uint> m_free_threads;

        uint m_num_threads;


        /**
         * Wait for messages to arrive in the queue and then process them. 
         */
        void run(PROCESSOR *p)
        {
            while (true)
            {
                try
                {
                    try
                    {
                        auto m = m_in_queue->dequeue();
                        monitor::set_value(m_available_threads_monitor_id, --m_free_threads);
                        try
                        {
                            p->process(m);
                            monitor::set_value(m_available_threads_monitor_id, ++m_free_threads);
                        }
                        catch (...)
                        {
                            monitor::set_value(m_available_threads_monitor_id, ++m_free_threads);
                            throw;
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
                    e.log(::boost::log::trivial::error, "exception caught in handler_pool::run");
                }
            }
        }

    };
}
