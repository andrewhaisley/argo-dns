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

#include <chrono>
#include <map>

#include <boost/uuid/uuid_generators.hpp>

#include "token_store.hpp"

using namespace std;
using namespace adns;
using namespace chrono;

#define ONE_DAY (24 * 60 * 60)

map<pair<ip_address, uuid>, time_point<steady_clock>> tokens;

uuid token_store::generate(const ip_address &source_ip)
{
    uuid u = boost::uuids::random_generator()();
    tokens[pair<ip_address, uuid>(source_ip, u)] = steady_clock::now();
    return u;
}

bool token_store::contains(const ip_address &source_ip, uuid token)
{
    auto k = pair<ip_address, uuid>(source_ip, token);

    if (tokens.find(k) == tokens.end())
    {
        return false;
    }

    auto age = duration_cast<seconds>(steady_clock::now() - tokens[k]).count();

    if (age >= ONE_DAY)
    {
        tokens.erase(k);
        return false;
    }
    else
    {
        return true;
    }
}
