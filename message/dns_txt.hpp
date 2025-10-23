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
