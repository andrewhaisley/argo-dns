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
#include <map>

#include "table.hpp"
#include "types.hpp"

namespace adns
{
    /**
     * cache of instances of something
     */
    template <class CLASS> class cache final
    {
    public:

        // note that this must be a map rather than an unordered map as sort
        // order is important
        typedef std::map<uuid, std::shared_ptr<CLASS>> cache_map_type;

        /** 
         * new cache instance - no instances loaded to start with
         */
        cache() : m_items(nullptr)
        {
        }

        /**
         * get the contents of the cache in the form of a key->value map
         */
        std::shared_ptr<std::map<uuid, std::shared_ptr<CLASS>>> items()
        {
            std::lock_guard<std::recursive_mutex> guard(m_lock);
            validate();
            return m_items;
        }

        /**
         * get a single instance or nullptr if none foound by key
         */
        std::shared_ptr<CLASS> get(uuid k)
        {
            std::lock_guard<std::recursive_mutex> guard(m_lock);
            validate();
            auto i = m_items->find(k);
            if (i == m_items->end())
            {
                return std::shared_ptr<CLASS>();
            }
            else
            {
                return i->second;
            }
        }

        /**
         * delete a single instance, if found, by key
         */
        void delete_row(uuid k)
        {
            std::lock_guard<std::recursive_mutex> guard(m_lock);
            m_items->erase(k);
        }

        /**
         * add a single instance to the cache 
         */
        void add(uuid k, const std::shared_ptr<CLASS> &v)
        {
            std::lock_guard<std::recursive_mutex> guard(m_lock);
            (*m_items)[k] = v;
        }

        /**
         * to be called when there has been a change in a specific table. default functionality
         * is to invalidate regardless of which table it is. Override to be more specific.
         */
        virtual void table_changed(db::table::table_t)
        {
            std::lock_guard<std::recursive_mutex> guard(m_lock);
            m_items.reset();
        }

    private:

        std::recursive_mutex m_lock;
        std::shared_ptr<cache_map_type> m_items;

        /**     
         * check if the cache is valid and, if not, load it
         */
        void validate()
        {
            if (!m_items)
            {
                m_items = std::shared_ptr<cache_map_type>(new cache_map_type());
                CLASS::load_cache();
            }
        }

    };
}
