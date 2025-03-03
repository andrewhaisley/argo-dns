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
