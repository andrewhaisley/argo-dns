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
#include "json.hpp"
#include "nullable.hpp"

namespace adns
{
    class json_serializable
    {
    public:

        /**
         * default constructor 
         */
        json_serializable();

        /**
         * destructor 
         */
        virtual ~json_serializable();

        /**
         * produce a JSON representation of the object
         */
        virtual json to_json() const final;

        /**
         * reset an existing instance from a JSON serialised form of another instance
         */
        virtual void from_json(const json &j) final;

    protected:

        mutable std::shared_ptr<json> m_json_object;

        /**
         * implemented by derived classes
         */
        virtual void json_serialize() const = 0;

        /**
         * implemented by derived classes
         */
        virtual void json_unserialize() = 0;

        /**
         * set a single scalar value in the base level object
         */
        template<typename T> void set_json(const std::string &name, T value) const
        {
            if (!m_json_object)
            {
                m_json_object = std::make_shared<json>(json::object_e);
            }
            (*m_json_object)[name] = value;
        }

        /**
         * set a single nullable scalar value in the base level object
         */
        template<typename T> void set_json(const std::string &name, nullable<T> value) const
        {
            if (!m_json_object)
            {
                m_json_object = std::make_shared<json>(json::object_e);
            }
            if (value.is_null())
            {
                (*m_json_object)[name] = json(json::null_e);
            }
            else
            {
                (*m_json_object)[name] = value.value();
            }
        }
    };
}
