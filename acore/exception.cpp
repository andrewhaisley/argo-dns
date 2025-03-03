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
#include <string.h>

#include <boost/uuid/uuid_generators.hpp>

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
    m_uuid = boost::uuids::nil_generator()();
}

adns::exception::exception(const char *method, const char *file, int line, const string &description, uuid oid)
{
    strncpy(m_method, method, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_file, file, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_description, description.c_str(), MAX_EXCEPTION_TEXT_LEN - 1);
    m_method[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_file[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_description[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_code = 0;
    m_line = line;
    m_details[0] = 0;
    m_uuid = oid;
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
    m_uuid = boost::uuids::nil_generator()();
}

adns::exception::exception(const char *method, const char *file, int line, const string &description, const string &details, uuid oid)
{
    strncpy(m_method, method, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_file, file, MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_description, description.c_str(), MAX_EXCEPTION_TEXT_LEN - 1);
    strncpy(m_details, details.c_str(), MAX_EXCEPTION_TEXT_LEN - 1);
    m_method[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_file[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_description[MAX_EXCEPTION_TEXT_LEN - 1] = 0;
    m_code = 0;
    m_line = line;
    m_uuid = oid;
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

uuid adns::exception::get_uuid() const noexcept
{
    return m_uuid;
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
        << get_code() << "|"
        << to_string(m_uuid); 
}
