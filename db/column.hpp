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

#include <string>

#include "types.hpp"

namespace adns
{
    namespace db
    {
        class column final
        {
        public:

            typedef enum
            {
                invalid_e,
                bool_e,
                octet_e,
                int_e,
                uint_e,
                uuid_e,
                datetime_e,
                string_e
            }
            type_t;

            /**
             * constructor - invalid column
             */
            column();
        
            /**
             * constructor
             */
            column(type_t type, const std::string &name, int position, bool nullable);

            /**
             * destruction
             */
            virtual ~column();

            // get methods
            type_t get_type() const;
            const std::string &get_name() const;
            int get_position() const;
            bool get_nullable() const;

        private:

            type_t       m_type;
            std::string  m_name;
            int          m_position;
            bool         m_nullable;

        };
    }
}
