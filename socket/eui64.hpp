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
