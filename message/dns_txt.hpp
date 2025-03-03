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

#include "types.hpp"
#include "dns_label.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(txt_format_exception, exception)

namespace adns
{
    class dns_txt final
    {
    public:

        friend class dns_message_serial;
    
        /**
         * empty record
         */
        dns_txt();

        /**
         * constructor from a string stored in the DB format. Format being base64 string encoded with
         * length bytes followed by data.
         */
        dns_txt(const std::string &s);

        /**
         * to base64 string, the format used in the DB, in json etc.
         */
        std::string to_base64() const;

        /**
         * append string
         */
        void append(const std::shared_ptr<dns_label> &string);

        /**
         * destructor
         */
        virtual ~dns_txt();

        /**
         * for debug
         */
        std::string to_string() const;

    private:

        std::list<std::shared_ptr<dns_label>> m_strings;
    };

    std::ostream &operator<<(std::ostream& stream, const dns_txt &t);
}
