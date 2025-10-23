#!/usr/bin/python3

#
#  Copyright 2025 Andrew Haisley
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
#  associated documentation files (the “Software”), to deal in the Software without restriction, 
#  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
#  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
#  subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all copies or substantial 
#  portions of the Software.
#
#  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
#  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
#  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
#  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
#  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

import sys
import os

def expand_template(template, class_name, result):
    s = open(template).read()
    s = s.replace('#class#', class_name)
    open(result, 'w').write(s)

def create_files(class_name):
    header = class_name + '.hpp'
    if os.path.isfile(header):
        print(header, 'already exists, aborting')
        return
    impl = class_name + '.cpp'
    if os.path.isfile(impl):
        print(impl, 'already exists, aborting')
        return
    expand_template('template_hpp', class_name, header)
    expand_template('template_cpp', class_name, impl)

for cn in sys.argv[1:]:
    create_files(cn)
