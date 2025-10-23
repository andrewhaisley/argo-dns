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
