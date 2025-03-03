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

#include <map>

#include "types.hpp"
#include "exception.hpp"
#include "json_serializable.hpp"
#include "dns_svcparam.hpp"

EXCEPTION_CLASS(svcparams_format_exception, exception)

namespace adns
{
    class dns_svcparams : public json_serializable 
    {
    public:

        friend class dns_message_serial;

        /**
         * Constructor 
         */
        dns_svcparams();

        /**
         * constructor from a string stored in the DB format - i.e. JSON
         */
        dns_svcparams(const std::string &s);

        /**
         * constructor from a JSON object.
         */
        dns_svcparams(const json &j);

        /**
         * Destructor 
         */
        virtual ~dns_svcparams();

        /**
         * for debug
         */
        std::string to_string() const;

    protected:

        std::map<dns_svcparam::svcparam_type_t, dns_svcparam> m_params;

        /**
         * Turn the instance into JSON
         */
        virtual void json_serialize() const;

        /**
         * Re-initialize the instance from json.
         */
        virtual void json_unserialize();

    };

    std::ostream &operator<<(std::ostream& stream, const dns_svcparams &t);
}
