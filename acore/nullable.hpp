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
