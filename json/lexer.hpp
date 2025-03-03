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

#include "token.hpp"
#include "reader.hpp"

namespace adns
{
    /**
     * \brief A lexical analyser for JSON messages. 
     *
     * A lexical analyser for JSON messages. Lexemes are generated from source
     * that matches the RFC but note that doesn't mean those lexemes can
     * always be represented as basic C++ types.
     */
    class lexer
    {
    public:

        /**
         * New lexer that will read tokens of up to the maximum
         * specified length from the supplied reader.
         */
        lexer(reader &r, size_t max_token_length);

        /**
         * Destructor.
         */
        virtual ~lexer();

        /**
         * Read the next token.
         * \throw json_parser_exception if a syntax error is encountered or
         *        if the token length exceeds the maximum.
         * \throw json_io_exception in the event of a IO error error of some
         *        type when reading the message.
         */
        const token &next();

        /**
         * Put back the last token that was read such that a subsequent call to
         * next() will return that token rather than attempting to read a new one
         * from the input.
         */
        void put_back_last();

    private:

        /**
         * Read and discard all whitespace as defined by the standard and
         * leave the reader pointing at the next relevant characater.
         */
        void consume_white_space();

        /**
         * Read the next token and leave it in the m_token member variable
         * for return by the next() method.
         */
        void read_token();

        /**
         * Read a number - integer or floating point - checking that it is
         * syntactically correct but not converting it into a C++ type other
         * than string.
         */
        void read_number();

        /**
         * Read a string encoding the result as UTT-8.
         */
        void read_string();

        /**
         * Read the "false" keyword (based on the first letter found being
         * an 'f').
         */
        void read_false();

        /**
         * Read the "true" keyword (based on the first letter found being
         * an 't').
         */
        void read_true();

        /**
         * Read the "null" keyword (based on the first letter found being
         * an 'n').
         */
        void read_null();

        /**
         * Read characters from the reader checking that they match the
         * the provided string.
         * \throw   json_parser_exception if there's a mismatch
         */
        void read_matching(const char *s);

        /**
         * Append a single character to a buffer holding the string
         * representation of the number we're parsing checking the
         * length for overflow.
         * \throw   json_parser_exception if there are too many characters.
         */
        void append_to_number_buffer(char *s, size_t &index, int c);
    
        /**
         * Read numerical digits (0-9) from the stream until a non-number
         * is found. The results go into m_buffer from the supplied index
         * position.
         */
        size_t read_digits(size_t &index);

        /**
         * Throw an appropriate exception based on the value of c
         * (unexpected EOF or unexpected character).
         */
        void throw_number_exception(int c);

        /// Reader to read from...
        reader &m_reader;

        /// true if last token was put back, false otherwise.
        bool m_last_put_back;

        /// Most recently read token,
        token m_token;

        /// Maximum allowed length of a token in characters.
        size_t m_max_token_length;

        /*
         * Buffer holding the token raw value. This is dynamically allocated so
         * as to avoid placing a very large object on the stack.
         */
        char *m_buffer;
    };
}
