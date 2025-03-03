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

#include "json.hpp"
#include "writer.hpp"

namespace adns
{
    /**
     * Write the JSON object to a stream. All output will be on one line. 
     * See below for more options.
     */
    std::ostream &operator<<(std::ostream &stream, const json &e);

    /**
     * Write the JSON object to a string. All output will be on one line. 
     * See below for more options.
     */
    void operator<<(std::string &s, const json &e);

    /**
     * \brief Class to unparse json instances into JSON messages.
     *
     * Class to unparse json instances into JSON messages. Various convenience
     * methods are supplied for different IO models.
     */
    class unparser
    {
    public:

        /**
         * Unparse a json instance to an ostream.
         * \param o             The ostream to write to.
         * \param j             The json instance to write.
         * \param space         What to print for the spacing around ":" and after ",".
         * \param newline       What to print for a newline after name/value pairs and array items.
         * \param indent        What to print for indents after a newline.
         * \param indent_inc    How much to increment by when indenting a new object or array.
         *                      1 == one indent string parameter value.
         * \throw json_io_exception Thrown in the case of a IO error whilst writing.
         */
        static void unparse(
                std::ostream &o,
                const json   &j,
                const char   *space = " ",
                const char   *newline = "",
                const char   *indent = " ",
                int          indent_inc = 0);

        /**
         * Unparse a json instance to a POSIX file descriptor. See documentation
         * for unparse to stream for other parameter details.
         * \throw json_io_exception Thrown in the case of a IO error whilst writing.
         */
        static void unparse(
                int        fd,
                const json &j,
                const char *space = " ",
                const char *newline = "",
                const char *indent = " ",
                int        indent_inc = 0);

        /**
         * Unparse a json instance to a stdio FILE. See documentation
         * for unparse to stream for other parameter details.
         * \throw json_io_exception Thrown in the case of a IO error whilst writing.
         */
        static void unparse(
                FILE       *s,
                const json &j,
                const char *space = " ",
                const char *newline = "",
                const char *indent = " ",
                int        indent_inc = 0);

        /**
         * Convenience method. Create a new file, write a single JSON
         * message to it and then close it.  See documentation for unparse 
         * to stream for other parameter details.
         * \throw json_io_exception Thrown in the case of a IO error whilst writing.
         */
        static void save(
                const json        &j,
                const std::string &file_name,
                const char        *space = " ",
                const char        *newline = "",
                const char        *indent = " ",
                int               indent_inc = 0);

        /**
         * Construct a new unparser given a writer instance.
         * \param w             The writer to use for output.
         * \param space         What to print for the spacing around ":" and after ",".
         * \param newline       What to print for a newline after name/value pairs and array items.
         * \param indent        What to print for indents after a newline.
         * \param indent_inc    How much to increment by when indenting a new object or array.
         *                      1 == one indent string parameter value.
         * \throw json_io_exception Thrown in the case of a IO error whilst writing.
         */
        unparser(writer     &w,
                 const char *space = " ",
                 const char *newline = "",
                 const char *indent = " ",
                 int        indent_inc = 0);

    
        /**
         * Unparse a single json instance.
         * \param j             The json instance to output.
         * \param indent_level  The indent level to start at (usually 0).
         * \throw json_io_exception Thrown in the case of a IO error whilst writing.
         */
        void unparse(const json &j, int indent_level = 0);

    private:

        void print_indent(int indent_level);
        void unparse_object(const json &j, int indent_level);
        void unparse_array(const json &j, int indent_level);

        writer     &m_writer;
        const char *m_space;
        const char *m_newline;
        const char *m_indent;
        int        m_indent_inc;

    };

}
