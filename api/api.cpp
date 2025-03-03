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
#include "api.hpp"
#include "api_url_v1.hpp"

using namespace std;
using namespace adns;

api::api()
{
}

api::~api()
{
}

shared_ptr<http_response> api::handle_request(shared_ptr<http_request> &req)
{
    api_url_v1 u(req->get_url().get_raw());

    switch (u.get_version())
    {
    case 1:
        return m_v1.handle_request(req, u);
    default:
        THROW(api_version_exception, "unsupported API version requested", u.get_version());
    }
}
