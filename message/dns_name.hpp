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

#include <list>

#include "dns_label.hpp"
#include "types.hpp"

namespace adns
{
    /**
     * names are lists of strings
     */
    class dns_name final
    {
    public:

        friend class dns_message_parser;
        friend class dns_message_unparser;
        friend class dns_message_serial;

        // the root domain
        static const dns_name root;

        /**
         * empty name
         */
        dns_name();

        /**
         * copy constructor - shallow copy
         */
        dns_name(const dns_name &other);

        /**
         * construct from a string in aaa.bbb.ccc etc format.
         */
        dns_name(const std::string &s);

        /**
         * destructor
         */
        virtual ~dns_name();

        /**
         * make a clone of the name
         */
        std::shared_ptr<dns_name> clone() const;

        /**
         * add label to the end of the name
         */
        void append(const std::shared_ptr<dns_label> &s);

        /**
         * add another name to the end of the name
         */
        void append(const dns_name &n);

        /**
         * remove first label from the name
         */
        void remove_first();

        /** 
         * remove the last N labels from the name - not very efficient, use with care
         */
        void remove_last_n_labels(size_t n);

        /**
         * add a label to the front of the name
         */
        void prepend(const std::shared_ptr<dns_label> &s);

        /**
         * add some labels to the front of the name
         */
        void prepend(const std::list<std::shared_ptr<dns_label>> &labels);

        /**
         * get the individual labels
         */
        const std::list<std::shared_ptr<dns_label>> &get_labels() const;

        /**
         * get the number of labels
         */
        size_t size() const;

        /**
         * true if we are a subdomain of other name
         * e.g. if we are a.b.c.com and other is b.c.com then this method returns true.
         * returns false if the names are equal
         */
        bool is_subdomain_of(const dns_name &other) const;

        /**
         * get the first N labels - not very efficient, use with care
         */
        std::list<std::shared_ptr<dns_label>> get_labels(size_t n) const;

        /**
         * is this the root domain?
         */
        bool is_root() const;

        /**
         * assignment operator - shallow copy
         */
        const dns_name &operator=(const dns_name &other);

        /**
         * == operator
         */
        bool operator==(const dns_name &other) const;

        /**
         * != operator
         */
        bool operator!=(const dns_name &other) const;

        /**
         * < operator
         */
        bool operator<(const dns_name &other) const;

        /**
         * equality allowing for this name being a wildcard
         */
        bool wildcard_equals(const dns_name &non_wildcard_other) const;

        /**
         * clear the name - remove all elements
         */
        void clear();

        /**
         * generate a hash for use in STL containers
         */
        size_t get_hash() const;

        /**
         * turn into printable string
         */
        std::string to_string() const;

    private:

        // hash value
        mutable size_t m_hash;

        std::list<std::shared_ptr<dns_label>> m_labels;
    };

    std::ostream &operator<<(std::ostream& stream, const dns_name &n);
}

/**
 * add a template specialisation so that dns_name can be the key in a hash map
 * without the need to specify an explict hash class
 */
namespace std 
{
    template <> struct hash<adns::dns_name>
    {
        size_t operator()(const adns::dns_name &k) const
        {
            return k.get_hash();
        }
    };
}
