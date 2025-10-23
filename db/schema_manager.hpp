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

#include "exception.hpp"
#include "types.hpp"
#include "connection.hpp"

EXCEPTION_CLASS(schema_manager_exception, exception)

namespace adns
{
    namespace db
    {
        class schema_manager
        {
        public:
            schema_manager() = delete;

            /**
             * Update the schema or, if it doesn't exist at all, create it from scratch
             */
            static void update_or_create();

        private:

            /**
             * Create the ADNS schema in whatever DB we're using.
             */
            static void create(connection &conn);

            /**
             * If the schema version in the DB is lower than the version required
             * for this server version, update it.
             */
            static void update(connection &conn, const std::string &v);

            /**
             * Create the static data needed for a base install 
             */
            static void create_static(connection &conn);
        };
    }
}
