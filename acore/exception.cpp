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
 
#include <string.h>

#include "types.hpp"
#include "exception.hpp"

using namespace std;
using namespace adns;
using namespace boost::log;
using namespace boost::log::trivial;

adns::exception::exception(const char *method, const char *file, int line, const string &description, int error_code)
{
    strncpy(m_method, method, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_file, file, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_description, description.c_str(), MAX_EXCEPTION_TEXT_LEN - 1);
    m_method[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_file[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_description[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_code = error_code;
    m_line = line;
    m_details[0] = 0;
}

adns::exception::exception(const char *method, const char *file, int line, const string &description, const string &details, int error_code)
{
    strncpy(m_method, method, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_file, file, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_description, description.c_str(), MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_details, details.c_str(), MAX_EXCEPTION_TEXT_LEN - 1);
    m_method[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_file[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_description[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_code = error_code;
    m_line = line;
}

adns::exception::~exception() noexcept(true)
{
}

const char* adns::exception::what() const noexcept
{
    return m_description;
}

const char* adns::exception::get_details() const noexcept
{
    return m_details;
}

const char* adns::exception::get_description() const noexcept
{
    return m_description;
}

int adns::exception::get_code() const noexcept
{
    return m_code;
}

const char *adns::exception::get_method() const noexcept
{
    return m_method;
}

const char *adns::exception::get_file() const noexcept
{
    return m_file;
}

int adns::exception::get_line() const noexcept
{
    return m_line;
}

void adns::exception::log(severity_level l, const char *message) const noexcept
{
    BOOST_LOG_STREAM_WITH_PARAMS(logger::get(), (keywords::severity = l)) 
        << message << "|" 
        << get_file() << "|" 
        << get_line() << "|" 
        << get_method() << "|" 
        << get_description() << "|" 
        << get_details() << "|" 
        << get_code();
}
