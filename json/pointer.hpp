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

#include <list>
#include <string>

namespace adns
{
    /**
     * \brief JSON pointers as per the RFT. 
     *
     * Instances of this class hold a representation of a JSON pointer, the logic to
     * apply it is held within the json class.
     */
    class pointer
    {
    public:

        /**
         * \brief Tokens in a JSON pointer.
         *
         * A pointer is make up of a list of tokens representing one of:
         *     all of the document
         *     a named slot in an object
         *     an index into an array.
         */
        class token
        {
        public:

            /// Different types of pointer tokens.
            typedef enum { all_e, object_e, array_e } type_t;

            /// Construct a token of type all_e.
            token();

            /// Construct a token of type object_e with a named slot.
            token(const std::string &name);

            /// Construct a token of type array_e with an array index.
            token(size_t index);

            /// Get the type of the token.
            type_t get_type() const;

            /// Get the name of the slot referenced.
            const std::string &get_name() const;

            /// Get the index in the array referenced.
            size_t get_index() const;

        private:

            /// Type of the token.
            type_t m_type;

            /// Name of the slot for an object_e token.
            std::string m_name;

            /// Index into the array for an array_e token.
            size_t m_index;
        };

        /**
         * New pointer. Parse the given path as part of construction.
         * \throw   json_pointer_exception if the pointer argument doesn't represent a valid path.
         */
        pointer(const std::string &pointer);

        /**
         * Get the list of tokens representing the pointer.
         */
        const std::list<token> &get_path() const;

    private:

        std::list<token> m_path;

        void build_from_uri_fragment(const std::string &pointer);
        void build_from_json_string(const std::string &pointer);
        bool next_token(const std::string &pointer, size_t &start, size_t &end);
        token translate_uri_token(const std::string &pointer, size_t &start, size_t &end);
        token translate_jsonp_token(const std::string &pointer, size_t &start, size_t &end);
        token make_token(const std::string &s);
        int from_hex(const std::string &s, size_t index);
    };

    /// Write a (converted) string representation of the path to a stream - mainly for debug purposes.
    std::ostream &operator<<(std::ostream &stream, const pointer &p);
}
