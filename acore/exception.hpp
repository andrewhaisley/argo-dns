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

#include <exception>
#include <string>

#include "types.hpp"

#define MAX_EXCEPTION_TEXT_LEN 140

namespace adns
{
    /**
     * Base class for exceptions. All exceptions have a text description,
     * some also have an error code, which can represent different things
     * depending on the exception subclass. Note that the constructors
     * require strings as parameters. It's possible that creating the strings
     * in the calling code will throw a memory allocation exception. In that
     * case all bets are off and the server will exit. No attempt has or will
     * be made to recover from such exceptions.
     */
    class exception : public std::exception
    {
    public:

        /**
         * Description is whatever makes sense. error code will often
         * be a system error code, but could be something else for
         * different subclasses
         */
        exception(const char *method, const char *file, int line, const std::string &description, int error_code = -1);

        /**
         * Description is whatever makes sense. Object ID is the relavant object instance.
         */
        exception(const char *method, const char *file, int line, const std::string &description, uuid oid);

        /**
         * Description is whatever makes sense. error code will often
         * be a system error code, but could be something else for
         * different subclasses. Details add more context.
         */
        exception(const char *method, const char *file, int line, const std::string &description, const std::string &details, int error_code = -1);

        /**
         * Description is whatever makes sense. 
         * Details add more context.
         */
        exception(const char *method, const char *file, int line, const std::string &description, const std::string &details, uuid oid);

        /**
         * destructor   
         */
        virtual ~exception() noexcept(true);

        /**
         * get the name of the function/method where the exception was thrown from
         */
        virtual const char *get_method() const noexcept;

        /**
         * get the name of the file where the exception was thrown from
         */
        virtual const char *get_file() const noexcept;

        /**
         * get the line number of the file where the exception was thrown from
         */
        virtual int get_line() const noexcept;

        /**
         * get a textual description of the error
         */
        virtual const char *what() const noexcept;

        /**
         * get a textual description of the error
         */
        const char *get_description() const noexcept;

        /**
         * get more details of the error
         */
        const char *get_details() const noexcept;

        /**
         * get the error code - could mean all sorts of different things.
         */
        int get_code() const noexcept;

        /**
         * get the UUID - could mean all sorts of different things.
         */
        uuid get_uuid() const noexcept;

        /**
         * log the exception
         */
        void log(boost::log::trivial::severity_level l, const char *message) const noexcept;

    private:

        char m_method[MAX_EXCEPTION_TEXT_LEN];
        char m_file[MAX_EXCEPTION_TEXT_LEN];
        int  m_line;
        char m_description[MAX_EXCEPTION_TEXT_LEN];
        char m_details[MAX_EXCEPTION_TEXT_LEN];
        int m_code;
        uuid m_uuid;
    };
}

/**
 * Macro to easily define an exception sub-class
 */
#define EXCEPTION_CLASS(CLASS_NAME, BASE_CLASS) \
namespace adns \
{ \
    class CLASS_NAME : public BASE_CLASS \
    { \
    public: \
        CLASS_NAME(const char *method, const char *file, int line, const std::string &description, int error_code = -1) : adns::BASE_CLASS(method, file, line, description, error_code) \
        { \
        } \
        CLASS_NAME(const char *method, const char *file, int line, const std::string &description, const std::string &details, int error_code = -1) : adns::BASE_CLASS(method, file, line, description, details, error_code) \
        { \
        } \
        CLASS_NAME(const char *method, const char *file, int line, const std::string &description, uuid oid) : adns::BASE_CLASS(method, file, line, description, oid) \
        { \
        } \
        CLASS_NAME(const char *method, const char *file, int line, const std::string &description, const std::string &details, uuid oid) : adns::BASE_CLASS(method, file, line, description, details, oid) \
        { \
        } \
    };   \
}

#define THROW(CLASS, ...) throw CLASS(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
