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
