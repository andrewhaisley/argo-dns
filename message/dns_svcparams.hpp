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
