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

EXCEPTION_CLASS(thread_pool_timeout_exception, exception)

namespace adns
{
    template <class THREAD> class thread_pool final
    {
    public:
        /** 
         * empty pool - need to add some thread instances before use
         */
        thread_pool()
        {
        }

        /**
         * delete threads - assume they've been joined
         */
        virtual ~thread_pool()
        {
            for (auto iter : m_all_threads)
            {
                delete iter;
            }
        }

        /**
         * add a new running thread
         */
        void add(THREAD *t)
        {
            std::lock_guard<std::mutex> guard(m_lock);
            m_free_threads.push_back(t);
            m_all_threads.push_back(t);
            m_condition.notify_one();
        }

        /**
         * get a thread, removing it from the pool in the process. Call blocks until
         * a thread becomes available.
         */
        THREAD *get()
        {
            THREAD *t = nullptr;

            while (true)
            {
                {
                    std::unique_lock<std::mutex> guard(m_lock);

                    while (m_free_threads.size() == 0)
                    {
                        m_condition.wait(guard);
                        if (m_condition.wait_for(guard, std::chrono::seconds(2)) == std::cv_status::timeout)
                        {
                            THROW(thread_pool_timeout_exception, "timeout");
                        }
                    }

                    t = m_free_threads.back();
                    m_free_threads.pop_back();
                }

                return t;
            }
        }

        /**
         * put a thread back in the pool once free.
         */
        void put(THREAD *t)
        {
            std::lock_guard<std::mutex> guard(m_lock);
            m_free_threads.push_back(t);
            m_condition.notify_one();
        }

        /** 
         * join all the threads in the pool
         */
        void join()
        {
            for (auto iter : m_all_threads)
            {
                iter->join();
            }
        }


    private:

        std::list<THREAD *> m_free_threads;
        std::list<THREAD *> m_all_threads;

        std::mutex m_lock;
        std::condition_variable m_condition;

    };
}
