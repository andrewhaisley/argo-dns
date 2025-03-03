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
#include "column.hpp"

namespace adns
{
    namespace db
    {
        /**
         * table foreign keys
         */
        class foreign_key
        {
        public:

            /**
             * constructor
             */
            foreign_key(
                const std::string &referenced_table,
                const column      &referenced_column,
                const std::string &referencing_table,
                const column      &referencing_column);

            /**
             * destructor
             */
            virtual ~foreign_key();

            // get methods
            const std::string &get_referenced_table() const;
            const column &get_referenced_column() const;
            const std::string &get_referencing_table() const;
            const column &get_referencing_column() const;

        private:

            std::string m_referenced_table;
            column      m_referenced_column;
            std::string m_referencing_table;
            column      m_referencing_column;
        };
    }
}
