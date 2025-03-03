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

#include "types.hpp"
#include "exception.hpp"
#include "connection.hpp"

EXCEPTION_CLASS(transaction_exception, exception)

namespace adns
{
    namespace db
    {
        /**
         * transaction handling / guard
         */
        class transaction final
        {
        public:

            /** 
             * start a transaction
             */
            transaction(connection &c);

            /**
             * if the transaction has already been committed or rolled back, do nothing otherwise roll it back
             */
            virtual ~transaction();

            /**
             * commit the transaction - can only be called once, after which rollback can't be called either
             */
            void commit();

            /**
             * rollback the transaction - can only be called once, after which commit can't be called either
             */
            void rollback();
            
        private:

            connection &m_connection;
            bool m_committed;
            bool m_rolled_back;
    
        };
    }
}
