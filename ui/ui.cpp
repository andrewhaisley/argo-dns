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
#include <fstream>

#include "util.hpp"
#include "ui.hpp"

using namespace std;
using namespace adns;


ui::ui(const string &document_root) : m_document_root(document_root)
{
}

ui::~ui()
{
}

shared_ptr<http_response> ui::handle_get_request(shared_ptr<http_request> &req)
{
    string path = req->get_url().get_path();

    if (path == "")
    {
        path = "/index.html";
    }
    else if (path[path.size() - 1] == '/')
    {
        path += "index.html";
    }

    path = m_document_root + path;

    LOG(debug) << "file path is " << path;

    ifstream is(path);

    if (is)
    {
        buffer b;

        try
        {
            b = buffer(is);
        }
        catch (buffer_exception &e)
        {
            is.close();
            throw;
        }

        return make_shared<http_response>(0, http_response::ok_200, b, util::mime_type(path));
    }
    else
    {
        return make_shared<http_response>(0, http_response::not_found_404);
    }
}

shared_ptr<http_response> ui::handle_request(shared_ptr<http_request> &req)
{
    LOG(debug) << req->get_method() << " " << req->get_url().get_path();

    if (req->get_method() != "GET")
    {
        return make_shared<http_response>(0, http_response::method_not_allowed_405);
    }
    else
    {
        return handle_get_request(req);
    }
}
