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
