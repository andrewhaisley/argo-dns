#!/usr/bin/python3

import os
import sys
import json
import datetime

basedir = ''
outdir = ''

cpp_type_mapping = {}
cpp_param_type_mapping = {}
to_string_method_mapping = {}
from_json_method_mapping = {}
to_json_method_mapping = {}
to_db_method_mapping = {}
parse_method_mapping = {}
unparse_method_mapping = {}
store_type_mapping = {}
read_method_mapping = {}

def set_mappings(mappings):
    global cpp_type_mapping
    global cpp_param_type_mapping
    global to_string_method_mapping
    global from_json_method_mapping
    global to_json_method_mapping
    global to_db_method_mapping
    global parse_method_mapping
    global unparse_method_mapping
    global store_type_mapping
    global read_method_mapping
    cpp_type_mapping = mappings['cpp_type']
    cpp_param_type_mapping = mappings['cpp_param_type']
    to_string_method_mapping = mappings['to_string_method']
    from_json_method_mapping = mappings['from_json_method']
    to_json_method_mapping = mappings['to_json_method']
    to_db_method_mapping = mappings['to_db_method']
    parse_method_mapping = mappings['parse_method']
    unparse_method_mapping = mappings['unparse_method']
    store_type_mapping = mappings['store_type']
    read_method_mapping = mappings['read_method']

def read_file(name):
    f = open(os.path.join(basedir, name))
    res = f.read()
    f.close()
    return res

def write_file(fn, contents):
    f = open(os.path.join(outdir, fn), 'w')
    f.write(contents)
    f.close()

def indent(s, n):
    if s[-1] == '\n':
        last = '\n'
    else:
        last = ''
    bits = s.strip().split('\n')
    bits = [(' ' * n) + b for b in bits]
    return '\n'.join(bits) + last

def replace_basics(template, record_type, record_type_value):
    template = template.replace('%%RECORD_TYPE%%', record_type)
    template = template.replace('%%RECORD_TYPE_VALUE%%', str(record_type_value))
    template = template.replace('%%DATE_TIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))
    return template

def get_item(m, item_type):
    if item_type in m:
        return m[item_type]
    else:
        raise Exception('unknown item type', item_type)

def cpp_param_type(item_type):
    res = get_item(cpp_param_type_mapping, item_type)
    if res[-1] != '&':
        res += ' '
    return res

def cpp_type(item_type):
    return get_item(cpp_type_mapping, item_type)

def replace_name(m, item_name, item_type):
    return m[item_type].replace('%%ITEM%%', item_name)

def to_string_method(item_name, item_type):
    return replace_name(to_string_method_mapping, item_name, item_type)

def from_json(item_name, item_type, var_name):
    return replace_name(from_json_method_mapping, item_name, item_type)

def to_json(item_name, item_type):
    return replace_name(to_json_method_mapping, item_name, item_type)

def to_db(item_name, item_type):
    return replace_name(to_db_method_mapping, item_name, item_type)

def parse_method(item_type):
    return get_item(parse_method_mapping, item_type)

def store_type(item_type):
    return get_item(store_type_mapping, item_type)

def unparse_method(item_type):
    return get_item(unparse_method_mapping, item_type)

def read_method(item_name, item_type):
    return replace_name(read_method_mapping, item_name, item_type)

def get_decl(item_name, item_type):
    return cpp_param_type(item_type) + 'get_' + item_name + '() const;'

def get_param_decl(item_name, item_type):
    return cpp_param_type(item_type) + item_name

def get_mvar_decl(item_name, item_type):
    return cpp_type(item_type) + ' m_' + item_name + ';'

def set_decl(item_name, item_type):
    return 'void set_' + item_name + '(' + cpp_param_type(item_type) + item_name + ');'

def set_impl(record_type, item_name, item_type):
    return \
        'void dns_rr_' + record_type + '::set_' + item_name + '(' + cpp_param_type(item_type) + item_name + ')\n' + \
        '{\n' + \
        '    m_' + item_name + ' = ' + item_name + ';\n' + \
        '}'

def get_impl(record_type, item_name, item_type):
    return \
        cpp_param_type(item_type) + 'dns_rr_' + record_type + '::get_' + item_name + '() const\n' + \
        '{\n' + \
        '    return m_' + item_name + ';\n' + \
        '}'

def replace_set_method_decls(template, record_type_content):
    decls = ''
    for item in record_type_content:
        item_name, item_type = item
        decls += set_decl(item_name, item_type) + '\n'
    template = template.replace('%%SET_METHODS%%', indent(decls, 8)) 
    return template
    
def replace_get_method_decls(template, record_type_content):
    decls = ''
    for item in record_type_content:
        item_name, item_type = item
        decls += get_decl(item_name, item_type) + '\n'
    template = template.replace('%%GET_METHODS%%', indent(decls, 8)) 
    return template

def replace_member_var_decls(template, record_type_content):
    mvars = ''
    for item in record_type_content:
        item_name, item_type = item
        mvars += get_mvar_decl(item_name, item_type) + '\n'
    template = template.replace('%%MEMBER_VARS%%', indent(mvars, 8))
    return template

def replace_set_method_impls(template, record_type, record_type_content):
    impls = ''
    for item in record_type_content:
        item_name, item_type = item
        impls += set_impl(record_type, item_name, item_type) + '\n'
    template = template.replace('%%SET_METHODS%%', impls) 
    return template

def replace_get_method_impls(template, record_type, record_type_content):
    impls = ''
    for item in record_type_content:
        item_name, item_type = item
        impls += get_impl(record_type, item_name, item_type) + '\n'
    template = template.replace('%%GET_METHODS%%', impls) 
    return template

def replace_to_string_method_impl(template, record_type_content):
    impl = '(get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type()) + " " + boost::lexical_cast<string>(get_ttl())'
    for item in record_type_content:
        item_name, item_type = item
        impl += ' + " " + ' + to_string_method(item_name, item_type)
    impl += ';'
    template = template.replace('%%TO_STRING_IMPL%%', impl)

    impl = '(get_name() ? get_name()->to_string() : string("")) + " " + dns_rr::type_to_string(get_type())'
    for item in record_type_content:
        item_name, item_type = item
        impl += ' + " " + ' + to_string_method(item_name, item_type)
    impl += ';'
    template = template.replace('%%TO_STRING_NO_TTL_IMPL%%', impl)

    return template

def replace_json_serialize_method_impl(template, record_type_content):
    impls = ''
    for item in record_type_content:
        item_name, item_type = item
        impls += '    set_json("%s", %s);\n' % (item_name, to_json(item_name, item_type))
    template = template.replace('%%JSON_SERIALIZE%%', impls)
    return template

def replace_json_unserialize_method_impl(template, record_type_content):
    impls = ''
    for item in record_type_content:
        item_name, item_type = item
        impls += '    m_%s = %s;\n' % (item_name, from_json(item_name, item_type, 'm_json_object'))
    template = template.replace('%%JSON_UNSERIALIZE%%', impls)
    return template

def process_hpp(template, record_type, record_type_value, record_type_content):
    fn = 'dns_rr_' + record_type + '.hpp'
    print('generating hpp file', fn)
    template = replace_basics(template, record_type, record_type_value)
    template = replace_set_method_decls(template, record_type_content)
    template = replace_get_method_decls(template, record_type_content)
    template = replace_member_var_decls(template, record_type_content)
    write_file(fn, template)

def process_cpp(template, record_type, record_type_value, record_type_content):
    fn = 'dns_rr_' + record_type + '.cpp'
    print('generating cpp file', fn)
    template = replace_basics(template, record_type, record_type_value)
    template = replace_set_method_impls(template, record_type, record_type_content)
    template = replace_get_method_impls(template, record_type, record_type_content)
    template = replace_to_string_method_impl(template, record_type_content)
    template = replace_json_serialize_method_impl(template, record_type_content)
    template = replace_json_unserialize_method_impl(template, record_type_content)
    write_file(fn, template)

def process_rr(conf):
    for record_type, contents in conf.items():
        print('processing resource record type', record_type)
        if 'genclass' in contents and not contents['genclass']:
            # do nothing since there's predefined class files for this one
            pass
        else:
            record_type_value = contents['value']
            record_type_content = contents['content']
            hpp_template = read_file('dns_rr_template_hpp')
            cpp_template = read_file('dns_rr_template_cpp')
            process_hpp(hpp_template, record_type, record_type_value, record_type_content)
            process_cpp(cpp_template, record_type, record_type_value, record_type_content)

def process_base_rr(conf):
    type_enum = []
    type_to_name_map = ''
    name_to_type_map = ''

    for record_type, contents in conf.items():
        type_enum.append('            T_' + record_type + '_e = ' + str(contents['value']))
        type_to_name_map += '    { dns_rr::T_' + record_type + '_e, "' + record_type + '"},\n'
        name_to_type_map += '    { "' + record_type + '", dns_rr::T_' + record_type + '_e},\n'

    base_hpp_template = read_file('dns_rr_base_template_hpp')
    base_hpp_template = base_hpp_template.replace('%%DATE_TIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))
    base_hpp_template = base_hpp_template.replace('%%DNS_RR_ENUM_TYPES%%', ',\n'.join(type_enum))
    write_file('dns_rr.hpp', base_hpp_template)

    base_cpp_template = read_file('dns_rr_base_template_cpp')
    base_cpp_template = base_cpp_template.replace('%%DATE_TIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))

    base_cpp_template = base_cpp_template.replace('%%DNS_RR_TYPE_TO_NAME%%', type_to_name_map)
    base_cpp_template = base_cpp_template.replace('%%DNS_RR_NAME_TO_TYPE%%', name_to_type_map)

    write_file('dns_rr.cpp', base_cpp_template)


def replace_include(template, conf):
    inc = ''
    for record_type, contents in conf.items():
        inc += '#include "dns_rr_' + record_type + '.hpp"\n';
        if not ('genrow' in contents and not contents['genrow']):
            inc += '#include "row_rr_' + record_type.lower() + '.hpp"\n';
    template = template.replace('%%INCLUDE%%', inc) 
    return template

def replace_instantiate(template, conf):
    ins = ''
    for record_type, contents in conf.items():
        ins += 'case dns_rr::T_' + record_type + '_e:\n';
        if record_type == 'GENERIC':
            ins += 'default:\n'
        ins += '    return std::make_shared<dns_rr_' + record_type + '>();\n'
    template = template.replace('%%INSTANTIATE%%', indent(ins, 4)) 
    return template

def parse_items(contents):
    res = ''
    for item in contents:
        item_name, item_type = item
        res += 'r->set_' + item_name + '(' + parse_method(item_type) + ');\n'
    return indent(res, 8)

def parse_items(contents):
    res = ''
    for item in contents:
        item_name, item_type = item
        res += 'r->set_' + item_name + '(' + parse_method(item_type) + ');\n'
    return indent(res, 8)

def unparse_items(contents):
    res = ''
    for item in contents:
        item_name, item_type = item
        if item_type == 'name':
            res += unparse_method(item_type) + '(*(r->get_' + item_name + '()), raw, name_offsets, size, offset, compress);\n'
        elif item_type == 'uncompressed name':
            res += unparse_method(item_type) + '(*(r->get_' + item_name + '()), raw, name_offsets, size, offset, false);\n'
        else:
            upm = unparse_method(item_type)
            if upm == 'unparse_short':
                res += unparse_method(item_type) + '(r->get_' + item_name + '(), raw, offset);\n'
            else:
                res += unparse_method(item_type) + '(r->get_' + item_name + '(), raw, size, offset);\n'
    return indent(res, 8)

def replace_parse(template, conf):
    parse = ''
    for record_type, contents in conf.items():
        parse += 'case dns_rr::T_' + record_type + '_e:\n';
        if record_type == 'GENERIC':
            parse += 'default:\n'
        parse += '    {\n'
        parse += '        dns_rr_' + record_type + ' *r = dynamic_cast<dns_rr_' + record_type + ' *>(rr.get());\n';
        parse += parse_items(contents['content'])
        parse += '    }\n'
        parse += '    break;\n'
    template = template.replace('%%PARSE%%', indent(parse, 4)) 
    return template

def replace_unparse(template, conf):
    unparse = ''
    for record_type, contents in conf.items():
        unparse += 'case dns_rr::T_' + record_type + '_e:\n';
        if record_type == 'GENERIC':
            unparse += 'default:\n'
        unparse += '    {\n'
        unparse += '        const dns_rr_' + record_type + ' *r = dynamic_cast<const dns_rr_' + record_type + ' *>(&rr);\n';
        unparse += unparse_items(contents['content'])
        unparse += '    }\n'
        unparse += '    break;\n'
    template = template.replace('%%UNPARSE%%', indent(unparse, 4)) 
    return template

def read_items(contents):
    res = ''
    for item in contents:
        item_name, item_type = item
        res += 'rr->set_' + item_name + '(' + read_method(item_name, item_type) + ');\n'
    return indent(res, 12)

def replace_read(template, conf):
    read = ''
    for record_type, contents in conf.items():
        if 'genrow' in contents and not contents['genrow']:
            pass
        else:
            read += 'case dns_rr::T_' + record_type + '_e:\n';
            read += '    {\n'
            read += '        for (auto row : row_rr_%s::get_by_zone_id(*conn, zid))\n' % record_type.lower()
            read += '        {\n'
            read += '            auto rr = make_shared<dns_rr_%s>();\n' % record_type
            read += '            rr->set_name(make_shared<dns_name>(row->get_name()));\n'
            read += '            rr->set_ttl(row->get_ttl());\n'
            read += '            rr->set_id(row->get_rr_%s_id());\n' % record_type.lower()
            read += '            rr->set_zone_id(zid);\n'
            read += read_items(contents['content'])
            read += '            res.push_back(rr);\n'
            read += '        }\n'
            read += '    }\n'
            read += '    break;\n'
    template = template.replace('%%READ%%', indent(read, 8)) 
    return template

def replace_delete(template, conf):
    delete = ''
    for record_type, contents in conf.items():
        if 'genrow' in contents and not contents['genrow']:
            pass
        else:
            delete += 'case dns_rr::T_' + record_type + '_e:\n';
            delete += '    row_rr_%s::delete_by_zone_id(conn, zid);\n' % record_type.lower()
            delete += '    break;\n'
    template = template.replace('%%DELETE%%', indent(delete, 4)) 
    return template

def replace_delete_single(template, conf):
    delete = ''
    for record_type, contents in conf.items():
        if 'genrow' in contents and not contents['genrow']:
            pass
        else:
            delete += 'case dns_rr::T_' + record_type + '_e:\n';
            delete += '    row_rr_%s::delete_by_rr_%s_id(conn, rr->get_id());\n' % (record_type.lower(), record_type.lower())
            delete += '    break;\n'
    template = template.replace('%%DELETE_SINGLE%%', indent(delete, 4)) 
    return template

def insert_items(record_type, contents):
    res = ''
    for item in contents:
        item_name, item_type = item
        res += 'row.set_' + item_name + '(' + to_db(item_name, item_type) + ");\n"

    res += 'row.set_ttl(r->get_ttl());\n'
    res += 'row.set_rr_%s_id(r->get_id());\n' % record_type.lower()
    res += 'row.set_zone_id(r->get_zone_id());\n'
    return indent(res, 4)

def replace_insert(template, conf):
    insert = ''
    for record_type, contents in conf.items():
        if 'genrow' in contents and not contents['genrow']:
            pass
        else:
            insert += 'case dns_rr::T_' + record_type + '_e:\n'
            insert += '{\n'
            insert += '    dns_rr_%s *r = dynamic_cast<dns_rr_%s*>(rr.get());\n' % (record_type, record_type)
            insert += '    row_rr_%s row;\n' % record_type.lower()
            insert += '    row.set_new_primary_key_value();\n'
            insert += '    row.set_name(rr->get_name()->to_string());\n'
            insert += '    row.set_zone_id(r->get_zone_id());\n'
            insert += '    r->set_id(row.get_primary_key_value());\n'
            insert += insert_items(record_type, contents['content'])
            insert += '    row.insert_row(conn);\n'
            insert += '}\n'
            insert += 'break;\n'

    template = template.replace('%%INSERT%%', indent(insert, 12)) 
    return template

def replace_update(template, conf):
    insert = ''
    for record_type, contents in conf.items():
        if 'genrow' in contents and not contents['genrow']:
            pass
        else:
            insert += 'case dns_rr::T_' + record_type + '_e:\n'
            insert += '{\n'
            insert += '    dns_rr_%s *r = dynamic_cast<dns_rr_%s*>(rr.get());\n' % (record_type, record_type)
            insert += '    row_rr_%s row;\n' % record_type.lower()
            insert += '    row.set_new_primary_key_value();\n'
            insert += '    row.set_name(rr->get_name()->to_string());\n'
            insert += '    row.set_zone_id(r->get_zone_id());\n'
            insert += '    r->set_id(row.get_primary_key_value());\n'
            insert += insert_items(record_type, contents['content'])
            insert += '    row.update_row(conn);\n'
            insert += '}\n'
            insert += 'break;\n'

    template = template.replace('%%UPDATE%%', indent(insert, 12)) 
    return template

def replace_record_type_list(template, conf):

    rr_types = []
    num_rr_types = 0

    for record_type, contents in conf.items():
        if 'genrow' in contents and not contents['genrow']:
            pass
        else:
            num_rr_types += 1
            rr_types.append('dns_rr::T_' + record_type + '_e')

    template = template.replace('%%NUM_RECORD_TYPES%%', str(num_rr_types))
    template = template.replace('%%RECORD_TYPES%%', ', '.join(rr_types))

    return template

def replace_json_parse(template, conf):
    parse = ''
    for record_type, contents in conf.items():
        parse += 'case dns_rr::T_' + record_type + '_e:\n';
        if record_type == 'GENERIC':
            parse += 'default:\n'
        parse += '    {\n'
        parse += '        dns_rr_' + record_type + ' *r = dynamic_cast<dns_rr_' + record_type + ' *>(res.get());\n';
        parse += '        r->from_json(j);\n'
        parse += '    }\n'
        parse += '    break;\n'

    template = template.replace('%%FROM_JSON%%', indent(parse, 4)) 
    return template

def process_parser(conf):
    print('generating dns_rr_parser.cpp')
    template = read_file('dns_rr_parser_template_cpp')
    template = template.replace('%%DATE_TIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))
    template = replace_include(template, conf)
    template = replace_instantiate(template, conf)
    template = replace_parse(template, conf)
    template = replace_json_parse(template, conf)
    template = replace_unparse(template, conf)
    write_file('dns_rr_parser.cpp', template)

def process_dal(conf):
    print('generating dns_rr_dal.cpp')
    template = read_file('dns_rr_dal_template_cpp')
    template = template.replace('%%DATE_TIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))
    template = replace_include(template, conf)
    template = replace_read(template, conf)
    template = replace_delete(template, conf)
    template = replace_delete_single(template, conf)
    template = replace_insert(template, conf)
    template = replace_update(template, conf)
    template = replace_record_type_list(template, conf)
    write_file('dns_rr_dal.cpp', template)

def process_schema(conf):
    schema = {}

    for record_type, contents in conf.items():
        if 'genrow' in contents and not contents['genrow']:
            # do nothing since this is some kind of virtual record that isn't persisted anywhere
            pass
        else:
            table_name = 'rr_' + record_type.lower()

            columns = {}
            indexes = {}

            indexes['PRIMARY'] = [ [ table_name + '_id', True ] ]
            indexes['i_' + table_name + '_zone'] = [ [ 'zone_id', False] ]

            columns[table_name + '_id'] = { 'nullable' : False, 'type' : 'uuid' }
            columns['zone_id'] = { 'nullable' : False, 'type' : 'uuid' }
            columns['name'] = { 'nullable' : False, 'type' : 'string' }
            columns['ttl'] = { 'nullable' : False, 'type' : 'uint' }

            for item in contents['content']:
                item_name, item_type = item
                columns[item_name] = {}
                t, l = store_type(item_type)
                columns[item_name]['type'] = t
                if l is not None:
                    columns[item_name]['length'] = l
                columns[item_name]['nullable'] = False

            schema[table_name] = {}
            schema[table_name]['name'] = table_name
            schema[table_name]['columns'] = columns
            schema[table_name]['foreign_keys'] = {}
            schema[table_name]['indexes'] = indexes

    write_file('../db/resource-records.json', json.dumps(schema, indent=4, sort_keys=True))

if len(sys.argv) != 4:
    print('usage', sys.argv[0], 'type-config-file (e.g. typeconf.json) config-file (e.g. rrconf.json) outdir (e.g. .)')
    exit(1)
else:
    js = json.loads(open(sys.argv[1]).read())
    set_mappings(js)
    js = json.loads(open(sys.argv[2]).read())
    outdir = sys.argv[2]
    basedir = os.path.dirname(sys.argv[1])
    outdir = basedir
    process_base_rr(js)
    process_rr(js)
    process_schema(js)
    process_parser(js)
    process_dal(js)
    exit(0)
