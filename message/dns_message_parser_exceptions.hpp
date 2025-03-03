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

#include "exception.hpp"

EXCEPTION_CLASS(message_parser_exception, exception)

    EXCEPTION_CLASS(message_parser_format_exception, message_parser_exception)
        EXCEPTION_CLASS(message_serial_overrun_exception, message_parser_format_exception)
        EXCEPTION_CLASS(message_serial_format_exception, message_parser_format_exception)
        EXCEPTION_CLASS(rr_parser_format_exception, message_parser_format_exception)
    EXCEPTION_CLASS(message_parser_unsupported_exception, message_parser_exception)

EXCEPTION_CLASS(message_serial_parameter_exception, exception)

EXCEPTION_CLASS(message_unparser_exception, exception)
