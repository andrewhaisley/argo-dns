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
