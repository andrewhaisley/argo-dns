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

#include <vector>

#include "types.hpp"
#include "connection.hpp"
#include "ip_address.hpp"
#include "network.hpp"
#include "json_serializable.hpp"

namespace adns
{
    /**
     * lists of network and host addresses
     */
    class address_list final : public json_serializable 
    {
    public:

        /**
         * empty/invalid instance
         */
        address_list();

        /**
         * get by ID - returns nullptr if not found
         */
        static std::shared_ptr<address_list> get(uuid id);

        /**
         * get all address_lists
         */
        static const std::map<uuid, std::shared_ptr<address_list>> &get_all();

        /**
         * figure out if an address matches the list
         */
        bool match(const ip_address &addr) const;

        /**
         * delete from the DB
         */
        void delete_address_list_db(db::connection &conn);

        /**
         * delete from the cache
         */
        void delete_address_list_cache();

        /**
         * insert into the DB 
         */
        uuid insert_address_list_db(db::connection &conn);

        /**
         * insert into the cache
         */
        void insert_address_list_cache();

        /**
         * update an existing address list in the DB
         */
        void update_address_list_db(db::connection &conn);

        /**
         * update an existing address list in the cache
         */
        void update_address_list_cache();

        /**
         * clone the object - deep copy
         */
        std::shared_ptr<address_list> clone()const;

        /**
         * get the id of the object
         */
        uuid get_address_list_id() const;

        /**
         * called by the cache instance whenever a (re)load of data from the DB is needed
         */
        static void load_cache();

        /**
         * dump out for debug
         */
        void dump() const;

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

        std::map<uuid, ip_address> m_ips;
        std::map<uuid, network>    m_networks;

        uuid m_address_list_id;

        static std::map<uuid, std::shared_ptr<address_list>> o_cache;

        /**
         * add an IP
         */
        void add(uuid id, const ip_address &addr);

        /**
         * add a network
         */
        void add(uuid id, const network &addr);

    };
}
