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

#include <mutex>
#include <condition_variable>
#include "types.hpp"

namespace adns
{
    /**
     * One writer, many readers, writer has priority lock
     */
    class read_write_lock final
    {
    public:

        /** 
         * lock guard for write operations
         */
        class write_guard final
        {
        public:

            /**
             * constructor - locks in write mode
             */
            write_guard(read_write_lock &l);

            /**
             * unlocks
             */
            ~write_guard();

        private:

            read_write_lock &m_lock;
        };

        /** 
         * lock guard for read operations
         */
        class read_guard final
        {
        public:

            /**
             * constructor - locks in read mode
             */
            read_guard(read_write_lock &l);

            /**
             * unlocks
             */
            ~read_guard();

        private:

            read_write_lock &m_lock;
        };

        /**
         * constructor - not locked
         */
        read_write_lock();

        /**
         * destructor
         */
        virtual ~read_write_lock();

        /**
         * lock for read 
         */
        void lock_read();

        /**
         * unlock for read 
         */
        void unlock_read();

        /**
         * lock for write
         */
        void lock_write();

        /**
         * unlock for write
         */
        void unlock_write();

    private:

        std::mutex m_lock;
        std::condition_variable m_no_readers_or_writers;
        std::condition_variable m_no_writers;

        int m_num_readers;
        int m_num_writers;
        int m_num_writers_waiting;
    };
}
