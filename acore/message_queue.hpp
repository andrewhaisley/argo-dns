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
#include "monitor.hpp"

EXCEPTION_CLASS(message_queue_timeout_exception, exception)

namespace adns
{
    /**
     * A threadsafe queue of messages to be processsed.
     */
    template <class MESSAGE> class message_queue final
    {
    public:

        /**
         * Create a queue with the given maximum length. When the max length is reached, the oldest
         * messages are dropped to make space for new ones.
         */
        message_queue(std::string name, size_t max_queue_length) : 
                                m_name(name), 
                                m_max_queue_length(max_queue_length), 
                                m_queue_length(0),
                                m_monitor_length_id(monitor::add_thing("queue", name, "length", 0))
        {
            (void)monitor::add_thing("queue", name, "maximum length", max_queue_length);
        }

        /**
         * Get a message for processing - block until one arrives.
         */
        MESSAGE dequeue()
        {
            MESSAGE m;

            while (true)
            {
                {
                    std::unique_lock<std::mutex> guard(m_lock);

                    while (m_queue_length == 0)
                    {
                        if (m_condition.wait_for(guard, std::chrono::seconds(2)) == std::cv_status::timeout)
                        {
                            THROW(message_queue_timeout_exception, "timeout");
                        }
                    }

                    m = m_message_queue.back();
                    m_message_queue.pop_back();
                    m_queue_length--;
                    monitor::set_value(m_monitor_length_id, m_queue_length);
                }

                return m;
            }
        }

        /**
         * Destructor
         */
        virtual ~message_queue()
        {
        }

        /**
         * Add a message to the processing queue.
         * @return  true if there was space to enqueue, false otherwise
         */
        bool enqueue(MESSAGE m)
        {
            std::lock_guard<std::mutex> guard(m_lock);

            if (m_queue_length > m_max_queue_length)
            {
                LOG(warning) << "queue " << m_name << " filled, not adding message";
                return false;
            }
            else
            {
                m_message_queue.push_front(m);
                monitor::set_value(m_monitor_length_id, m_queue_length);
                m_queue_length++;
                m_condition.notify_one();
                return true;
            }
        }
    
        /** 
         * Get the name of the queue
         */
        const std::string &get_name() const
        {
            return m_name;
        }

    private:

        std::string m_name;

        size_t m_max_queue_length;
        size_t m_queue_length;
    
        uint m_monitor_length_id;

        std::list<MESSAGE>      m_message_queue;
        std::mutex              m_lock;
        std::condition_variable m_condition;
    };
}
