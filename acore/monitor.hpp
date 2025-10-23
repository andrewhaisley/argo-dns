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
