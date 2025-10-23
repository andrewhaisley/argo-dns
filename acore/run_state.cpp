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
