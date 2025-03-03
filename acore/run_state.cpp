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
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "run_state.hpp"

using namespace std;
using namespace adns;

atomic<run_state::state_t> run_state::o_state(run_state::run_e);
atomic<bool> run_state::o_restart(false);

static char **o_argv = nullptr;
static char **o_envp = nullptr;

void run_state::store_args(char **argv, char **envp)
{
    o_argv = argv;
    o_envp = envp;
}

void run_state::restart()
{
    if (execve(o_argv[0], o_argv, o_envp) == -1)
    {
        THROW(run_state_exception, "execve failed", strerror(errno));
    }
}

void run_state::set(state_t new_state)
{
    if (o_state == shutdown_e && new_state != shutdown_e)
    {
        THROW(run_state_exception, "can't reverse a shutdown");
    }
    else
    {
        o_state = new_state;
    }
}

string run_state::current_state_as_string()
{
    switch (static_cast<state_t>(o_state))
    {
    case run_e:
        return "run";
    case pause_e:
        return "pause";
    case shutdown_e:
        if (o_restart)
        {
            return "restart";
        }
        else
        {
            return "shutdown";
        }
    default:
        THROW(run_state_exception, "unrecognised run state");
    }
}

void run_state::set(const string &new_state)
{
    if (new_state == "run")
    {
        set(run_e);
    }
    else if (new_state == "pause")
    {
        set(pause_e);
    }
    else if (new_state == "shutdown")
    {
        set(shutdown_e);
    }
    else if (new_state == "restart")
    {
        o_restart = true;
        set(shutdown_e);
    }
    else
    {
        THROW(run_state_exception, "unrecognised run state name", new_state);
    }
}
