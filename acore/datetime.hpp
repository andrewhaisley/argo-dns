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

#include "time.h"
#include <string>

#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(datetime_exception, exception)

namespace adns
{
    /**
     * Very simple datetime class handling primarily for database related
     * things.
     */
    class datetime
    {
    public:

        /**
         * invalid datetime
         */
        datetime();

        /** 
         * destructor
         */
        virtual ~datetime();

        /**
         * from string in form yyyy-mm-dd hh:mm:ss
         */
        datetime(const std::string &s);

        /**
         * from a time_t
         */
        datetime(time_t t);

        /**
         * to string in form yyyy-mm-dd hh:mm:ss
         */
        const std::string &to_string() const;

        /**
         * to string in form Day, N Mon YYYY HH:MM:SS TZ
         */
        const std::string &to_http_string() const;

        /**
         * to a time_t
         */
        time_t to_time_t() const;

        /**
         * get the time now
         */
        static datetime now();

        /**
         * time, in seconds, between this datetime and another
         */
        time_t operator-(const datetime &other) const;

    private:

        struct tm m_time;
        struct tm m_gm_time;

        mutable std::string m_string_form;
        
    };

    std::ostream &operator<<(std::ostream& stream, const datetime &dt);
}
