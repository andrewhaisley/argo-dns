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
#include <vector>

#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(monitor_exception, exception)

namespace adns
{
    class monitor
    {
    public:

        typedef enum
        {
            uint_e,
            string_e
        }
        value_type_t;

        typedef struct
        {
            value_type_t type;
            std::string  monitor_class;
            std::string  monitor_instance;
            std::string  monitor_item;
        }
        info_t;

        monitor() = delete;

        static uint add_thing(
                    const std::string &monitor_class,
                    const std::string &monitor_instance,
                    const std::string &monitor_item,
                    uint              initial_value);

        static uint add_thing(
                    const std::string &monitor_class,
                    const std::string &monitor_instance,
                    const std::string &monitor_item,
                    const std::string &initial_value);

        static void set_value(uint id, uint value);
        static void set_value(uint id, const std::string &value);

        static std::vector<info_t> get_things();
        static std::vector<uint> get_uint_values();
        static std::vector<std::string> get_string_values();

    private:

        static uint o_next_id;

        static std::vector<info_t>      o_things;
        static std::vector<uint>        o_uint_values;
        static std::vector<std::string> o_string_values;

        static std::mutex o_lock;
        
    };
}
