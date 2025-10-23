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
#include <map>

#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(url_exception, exception)

namespace adns
{
    /**
     * URLs... 
     */
    class url
    {
    public:

        /**
         * default constructor - empty
         */
        url();

        /**
         * from string of form /blah?arg1=val1&arg2=val2 etc
         */
        url(const std::string &s);

        /** 
         * destructor
         */
        virtual ~url();

        /**
         * get the raw URL
         */
        const std::string &get_raw() const;

        /**
         * get the path component
         */
        const std::string &get_path() const;

        /**
         * get the value of a parameter
         */
        std::string get_parameter(const std::string &name, const std::string &default_value="") const;

        /**
         * get the value of all parameters
         */
        const std::map<std::string, std::string> get_parameters() const;

        /**
         * dump for debug
         */
        void dump() const;

    private:

        std::string m_raw;
        std::string m_path;
        std::map<std::string, std::string> m_parameters;
    };
}
