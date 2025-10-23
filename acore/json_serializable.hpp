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
