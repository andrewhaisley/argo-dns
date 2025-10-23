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
