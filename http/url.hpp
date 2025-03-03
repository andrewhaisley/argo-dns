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
