#!/usr/bin/python3

# 
# Copyright 2025 Andrew Haisley
# 
# This program is free software: you can redistribute it and/or modify it under the terms 
# of the GNU General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with this program. 
# If not, see https://www.gnu.org/licenses/.
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
