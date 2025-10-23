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
#include "ip_address.hpp"
#include "json_serializable.hpp"
#include "connection.hpp"
#include "address_list.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(dns_horizon_exception, exception);

namespace adns
{

    /**
     * horizons - lists of IP addresses and networks
     */
    class dns_horizon final : public json_serializable
    {
    public:

        /**
         * new empty/invalid horizon
         */
        dns_horizon();

        /**
         * find the horizon for an address. Note that we return a pointer rather than
         * a shared pointer as this operation is performed for every single DNS query
         * so performance is critical.
         * @return  nullptr if no match found
         */
        static dns_horizon *get(const ip_address &addr);

        /**
         * find the horizon from its id
         * @return  nullptr if no match found
         */
        static dns_horizon *get(uuid id);

        /**
         * get all horizons 
         */
        static const std::vector<dns_horizon *> &get_all();

        /** 
         * get the horizon ID
         */
        uuid get_horizon_id() const;

        /**
         * delete the horizon and its zones from the DB.
         */
        void delete_horizon_db(db::connection &conn);

        /**
         * delete the horizon and its zones from the cache
         */
        void delete_horizon_cache();

        /**
         * insert a horizon row into the DB 
         * return the UUID of the newly created row
         */
        uuid insert_horizon_db(db::connection &conn);

        /**
         * insert a horizon row into the cache
         */
        void insert_horizon_cache();

        /**
         * update an existing horizon in the DB
         */
        void update_horizon_db(db::connection &conn);

        /**
         * update an existing horizon in the cache
         */
        void update_horizon_cache();

        /**
         * allow the caller to make recursive queries?
         */
        bool allow_recursion() const;

        /**
         * clone the object - deep copy
         */
        dns_horizon *clone() const;

        /**
         * is the given address_list used by any horizon?
         */
        static bool address_list_used(const address_list &al);

        /**
         * called by the cache instance whenever a (re)load of data from the DB is needed
         */
        static void load_cache();

        /**
         * delete everything in the cache
         */
        static void empty_cache();

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

        // ordered by check_order
        static std::vector<dns_horizon *> o_cache;

        uuid m_horizon_id;
        uuid m_address_list_id;
        bool m_allow_recursion;
        uint m_check_order;

        /**
         * figure out if an address matches a horizon
         */
        bool match(const ip_address &addr);

    };
}
