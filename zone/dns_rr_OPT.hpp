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

#include "dns_rr.hpp"
#include "buffer.hpp"

namespace adns
{
    /**
     * OPT records aren't real in the sense that they contain protocol related information only
     * and are handled as a special case by the parsing code.
     */
    class dns_rr_OPT final : public dns_rr
    {
    public:

        /**
         * Constructor 
         */
        dns_rr_OPT();

        /**
         * Destructor 
         */
        virtual ~dns_rr_OPT();

        /**
         * set methods
         */
        void set_data(const buffer &data);

        /**
         * make a copy 
         */
        virtual dns_rr *clone();

        /**
         * get methods
         */
        const buffer &get_data() const;
        unsigned short get_payload_size() const;

        /**
         * dump out for debug
         */
        virtual void dump() const;

    private:

        /**
         * turn into JSON
         */
        void json_serialize() const;

        /**
         * build from JSON - does nothing in practise.
         */
        void json_unserialize();

        buffer m_data;

    };
}
