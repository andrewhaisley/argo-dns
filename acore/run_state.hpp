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
