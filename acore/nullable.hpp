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
 
#pragma once

#include "types.hpp"
#include "exception.hpp"

EXCEPTION_CLASS(nullable_exception, exception)

namespace adns
{
    /**
     * nullable values 
     */
    template <class CLASS> class nullable final
    {
    public:

        nullable()
        {
            m_is_null = true;
        }

        nullable(const CLASS &c)
        {
            m_value = c;
            m_is_null = false;
        }

        virtual ~nullable()
        {
        }

        bool is_null() const
        {
            return m_is_null;
        }

        CLASS value() const
        {
            if (m_is_null)
            {
                THROW(nullable_exception, "attempt to access null value");
            }
            else
            {
                return m_value;
            }
        }

        nullable<CLASS> &operator=(const CLASS &other)
        {
            m_value = other;
            m_is_null = false;
            return *this;
        }

    private:

        CLASS m_value;
        bool  m_is_null;
    };

    template<typename T> std::ostream &operator<<(std::ostream& stream, const nullable<T> &v)
    {
        if (v.is_null())
        {
            stream << "<NULL>";
        }
        else
        {
            stream << v.value();
        }
        return stream;
    }
}
