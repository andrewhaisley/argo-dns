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

#include <arpa/inet.h>
#include <string>

#include "exception.hpp"
#include "json_serializable.hpp"

EXCEPTION_CLASS(eui64_exception, exception)

namespace adns
{
    /**
     * IP4 & IP6 addresses
     */
    class eui64 : public json_serializable
    {
    public:

        friend class dns_message_serial;

        /**
         * Invalid address
         */
        eui64();

        /**
         * EUI48 address from string.
         */
        eui64(const std::string &s);

        /**
         * destructor
         */
        virtual ~eui64();

        /**
         * string representation
         */
        std::string to_string() const;

    protected:

        /**
         * serialize into a json object
         */
        virtual void json_serialize() const;

        /**
         * unserialize from a json object
         */
        virtual void json_unserialize();

    private:

        octet m_eui64[8];

        void init(const std::string &s);

    };

    std::ostream &operator<<(std::ostream& stream, const eui64 &addr);
}
