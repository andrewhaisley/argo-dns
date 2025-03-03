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

#include <ostream>

namespace adns
{
    /**
     * \brief Lexical tokens read from a JSON message. 
     *
     * A lexical token read from a JSON message. The underlying string
     * and number values are held in non-converted form to allow a caller
     * further up the stack to decide how to interpret them.
     */
    class token
    {
    public:

        /// Type of the token. Matches the RFC except that ints and floats are separated out.
        typedef enum 
        {
            /// [
            begin_array_e,
            /// {
            begin_object_e,
            /// ]
            end_array_e,
            /// }
            end_object_e,
            /// :
            name_separator_e,
            /// ,
            value_separator_e,
            /// whole number
            number_int_e,
            /// floating point number
            number_double_e,
            /// sting in JSON format
            string_e,
            /// false
            false_e,
            /// true
            true_e,
            /// null
            null_e
        } 
        token_type;

        /// Constructor. null token.
        token() noexcept;

        /// Copy constructor. Deep copy.
        token(const token &other);

        /// Move constructor. Shallow copy.
        token(token &&other);

        /**
         * Constructor given a type - used for the various structural tokens
         * where the type implies the textual representation.
         */
        token(token_type t);

        /**
         * Constructor given a type and a raw string value.
         * A copy is taken of the string data.
         */
        token(token_type t, const char *raw_value, size_t len);

        /**
         * Destructor.
         */
        virtual ~token();

        /// Assignment. Deep copy.
        token &operator=(const token &other);

        /// Move. Shallow copy.
        token &operator=(token &&other);

        /// Get the raw untranslated JSON value.
        const std::string &get_raw_value() const;

        /// Get the token type.
        token_type get_type() const;

    private:

        /// The raw untranslated value. E.g. "\u1234"
        std::string m_raw_value;

        /// Token type.
        token_type m_type;
    };
}
