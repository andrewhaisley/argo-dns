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

#include <mutex>

#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(label_exception, exception)

#define MAX_LABEL_LENGTH 64

namespace adns
{
    /**
     * Immutable strings of arbitrary octets used in domain names and resource records. 
     * Although it's unusual to have anything that isn't a normal ascii string
     * the standards allow anything, so here it is...
     */
    class dns_label final
    {
    public:

        friend class dns_message_parser;
        friend class dns_message_unparser;
        friend class dns_message_serial;
        friend class dns_name;
        friend class dns_txt;

        /**
         * empty string
         */
        dns_label();

        /**
         * destructor
         */
        virtual ~dns_label();

        /**
         * standard constructor - take a copy of the data
         */
        dns_label(const octet *s, octet size);

        /**
         * constructor from string 
         */
        dns_label(const std::string &s);

        /**
         * byte by byte comparison, except that upper and lower case versions of the same letters are
         * considered equal.
         * 
         */
        bool operator==(const dns_label &other) const;
        bool operator!=(const dns_label &other) const;

        /**
         * byte by byte comparison. Upper and lower case considered equal again.
         */
        bool operator<(const dns_label &other) const;

        /**
         * generate a hash for use in STL containers
         */
        size_t get_hash() const;

        /**
         * get the length of the label
         */
        octet get_length() const;

        /**
         * the wildcard label
         */
        static dns_label wildcard;

        /**
         * mainly for debug - turn into a printable string
         */
        std::string to_string() const;

    private:

        octet m_data[MAX_LABEL_LENGTH];
        octet m_size;
    };

    std::ostream &operator<<(std::ostream& stream, const dns_label &l);
}

/**
 * add a template specialisation so that dns_label can be the key in a hash map
 * without the need to specify an explict hash class
 */
namespace std 
{
    template <> struct hash<adns::dns_label>
    {
        size_t operator()(const adns::dns_label &k) const
        {
            return k.get_hash();
        }
    };
}
