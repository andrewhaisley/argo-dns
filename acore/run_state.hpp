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

#include <atomic>
#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(run_state_exception, exception)

namespace adns
{
    /**
     * global run state control - e.g. should we run normally, pause, shut down 
     */
    class run_state final
    {
    public:

        /**
         * no instantiation
         */
        run_state() = delete;

        typedef enum
        {
            run_e,
            pause_e,
            shutdown_e
        }
        state_t;

        // treat this as a read only variable
        static std::atomic<state_t> o_state;

        // treat this as a read only variable - set to true if we are going to restart after 
        // a shutdown
        static std::atomic<bool> o_restart;

        /**
         * store start-up arguments for use in restarts
         */
        static void store_args(char **argv, char **envp);

        /**
         * restart (exec)
         */
        static void restart();

        /**
         * set the run state - checks for valid transitions
         */
        static void set(state_t new_state);

        /**
         * set the run state from a string - checks for valid transitions
         */
        static void set(const std::string &new_state);

        /**
         * get a string describing the current state
         */
        static std::string current_state_as_string();
    };
}
