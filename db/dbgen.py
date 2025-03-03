#!/usr/bin/python3

import sys
import json
import datetime

def read_file(name):
    f = open(name)
    res = f.read()
    f.close()
    return res

def write_file(fn, contents):
    f = open(fn, 'w')
    f.write(contents)
    f.close()

def indent(s, n):
    if len(s) > 0 and s[-1] == '\n':
        last = '\n'
    else:
        last = ''
    bits = s.strip().split('\n')
    bits = [(' ' * n) + b for b in bits]
    return '\n'.join(bits) + last

def replace_basics(template, class_name, table_name):
    template = template.replace('%%CLASS%%', class_name)
    template = template.replace('%%TABLE_NAME%%', table_name)
    template = template.replace('%%DATETIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))
    return template

def get_var_type(data_type, nullable):
    if data_type == 'string':
        data_type = 'std::string'
    if nullable:
        return 'nullable<' + data_type + '>'
    else:
        return data_type

def replace_member_vars(header, columns):
    vars = ''
    for column_name, attrs in columns.items():
        var_name = 'm_' + column_name
        var_type = get_var_type(attrs['type'], attrs['nullable'])
        vars += var_type + ' ' + var_name + ';\n'
    header = header.replace('%%MEMBER_VARS%%', indent(vars, 12))
    return header

def replace_get_method_decls(header, columns):
    methods = ''
    for column_name, attrs in columns.items():
        var_type = get_var_type(attrs['type'], attrs['nullable'])
        methods += var_type + ' get_' + column_name + '() const;\n'
    header = header.replace('%%GET_METHODS%%', indent(methods, 12))
    return header

def replace_set_method_decls(header, columns):
    methods = ''
    for column_name, attrs in columns.items():
        var_type = get_var_type(attrs['type'], attrs['nullable'])
        methods += 'void set_' + column_name + '(' + var_type + ' v);\n'
    header = header.replace('%%SET_METHODS%%', indent(methods, 12))
    return header

def get_col_type_and_index(columns, col_name):
    pos = 1
    for column_name, attrs in columns.items():
        if column_name == col_name:
            var_type = get_var_type(attrs['type'], attrs['nullable'])
            return var_type, pos
        pos += 1
    raise Exception("didn't find column " + col_name)


def validate_index(attrs, columns, index_name, class_name):
    if len(attrs) > 2:
        raise Exception('more than two columns found in index ' + index_name + ' for class ' + class_name)

    for a in attrs:
        col_name, unique = a
        if col_name not in columns:
            raise Exception('column' + col_name + ' for class ' + class_name + ' in index but not table')
        col_type = columns[col_name]['type']
        if col_type not in ['uint', 'uuid']:
            raise Exception("key column " + col_name + " is not uint/uuid but " + col_type)

def get_method_params(columns, attrs):
    res = ''
    for a in attrs:
        col_name, unique = a
        if res != '':
            res += ', '
        res += columns[col_name]['type'] + ' ' + col_name
    return res
        

def replace_get_by_method_decls(header, class_name, columns, indexes):
    methods = ''
    for index_name, attrs in indexes.items():

        validate_index(attrs, columns, index_name, class_name)
        method_name = get_method_name(attrs, columns)
        params = get_method_params(columns, attrs)

        col_name, unique = attrs[0]

        if unique:
            methods += 'static std::shared_ptr<' + class_name + '> get_by_' + method_name + '(connection &c, ' + params +');\n'
        else:
            methods += 'static std::vector<std::shared_ptr<' + class_name + '>> get_by_' + method_name + '(connection &c, ' + params + ');\n'

    header = header.replace('%%GET_BY_METHODS%%', indent(methods, 12))
    return header

def replace_delete_by_method_decls(header, class_name, columns, indexes):
    methods = ''
    for index_name, attrs in indexes.items():

        validate_index(attrs, columns, index_name, class_name)
        method_name = get_method_name(attrs, columns)
        params = get_method_params(columns, attrs)

        methods += 'static void delete_by_' + method_name + '(connection &c, ' + params + ');\n'
    header = header.replace('%%DELETE_BY_METHODS%%', indent(methods, 12))
    return header

def replace_constructor(impl, columns):
    cons = []
    for column_name, attrs in columns.items():
        if not attrs['nullable']:
            var_type = attrs['type']
            if var_type in ['int', 'uint']:
                cons.append('m_' + column_name + ' = 0;')
            elif var_type == 'bool':
                cons.append('m_' + column_name + ' = false;')
    impl = impl.replace('%%CONSTRUCTOR%%', indent('\n'.join(cons), 4))
    return impl

def replace_set_methods_of_type(impl, columns, placeholder, type_name, is_null):
    set_cols =  'switch (c.get_position())\n'
    set_cols += '{\n'
    pos = 1
    n = 0
    for name, attrs in columns.items():
        if attrs['nullable'] == is_null and attrs['type'] == type_name:
            set_cols += 'case %s:\n' % pos
            set_cols += '    m_%s = value;\n' % name
            set_cols += '    break;\n'
            n += 1
        pos += 1
    set_cols += 'default:\n'
    set_cols += '    THROW(row_exception, "no columns of type ' + type_name + ' found at position", c.get_position());\n'
    set_cols += '}'

    if n == 0:
        return impl.replace(placeholder, '    (void)value;\n    THROW(row_exception, "no columns of type ' + type_name + ' found", c.get_position());')
    else:
        return impl.replace(placeholder, indent(set_cols, 4))

def replace_get_methods_of_type(impl, columns, placeholder, type_name, is_null):
    get_cols =  'switch (c.get_position())\n'
    get_cols += '{\n'
    pos = 1
    n = 0
    for name, attrs in columns.items():
        if attrs['nullable'] == is_null and attrs['type'] == type_name:
            get_cols += 'case %s:\n' % pos
            get_cols += '    return m_%s;\n' % name
            n += 1
        pos += 1
    get_cols += 'default:\n'
    get_cols += '    THROW(row_exception, "no columns of type ' + type_name + ' found at position", c.get_name(), c.get_position());\n'
    get_cols += '}'

    if n == 0:
        return impl.replace(placeholder, '    THROW(row_exception, "no columns of type ' + type_name + ' found", c.get_name(), c.get_position());')
    else:
        return impl.replace(placeholder, indent(get_cols, 4))
            
def replace_set_methods(impl, columns):
    defs = [
        ('%%SET_INT_COLUMN%%', 'int', False),
        ('%%SET_UNSIGNED_INT_COLUMN%%', 'uint', False),
        ('%%SET_OCTET_COLUMN%%', 'octet', False),
        ('%%SET_UUID_COLUMN%%', 'uuid', False),
        ('%%SET_BOOL_COLUMN%%', 'bool', False),
        ('%%SET_STRING_COLUMN%%', 'string', False),
        ('%%SET_DATETIME_COLUMN%%', 'datetime', False),
        ('%%SET_BUFFER_COLUMN%%', 'buffer', False),
        ('%%SET_NULLABLE_INT_COLUMN%%', 'int', True),
        ('%%SET_NULLABLE_UUID_COLUMN%%', 'uuid', True),
        ('%%SET_NULLABLE_UNSIGNED_INT_COLUMN%%', 'uint', True),
        ('%%SET_NULLABLE_OCTET_COLUMN%%', 'octet', True),
        ('%%SET_NULLABLE_BOOL_COLUMN%%', 'bool', True),
        ('%%SET_NULLABLE_STRING_COLUMN%%', 'string', True),
        ('%%SET_NULLABLE_DATETIME_COLUMN%%', 'datetime', True),
        ('%%SET_NULLABLE_BUFFER_COLUMN%%', 'buffer', True)];
    for placeholder, type_name, is_null in defs:
        impl = replace_set_methods_of_type(impl, columns, placeholder, type_name, is_null)
    return impl

def replace_get_methods(impl, columns):
    defs = [
        ('%%GET_INT_COLUMN%%', 'int', False),
        ('%%GET_UNSIGNED_INT_COLUMN%%', 'uint', False),
        ('%%GET_OCTET_COLUMN%%', 'octet', False),
        ('%%GET_UUID_COLUMN%%', 'uuid', False),
        ('%%GET_BOOL_COLUMN%%', 'bool', False),
        ('%%GET_STRING_COLUMN%%', 'string', False),
        ('%%GET_DATETIME_COLUMN%%', 'datetime', False),
        ('%%GET_BUFFER_COLUMN%%', 'buffer', False),
        ('%%GET_NULLABLE_INT_COLUMN%%', 'int', True),
        ('%%GET_NULLABLE_UUID_COLUMN%%', 'uuid', True),
        ('%%GET_NULLABLE_UNSIGNED_INT_COLUMN%%', 'uint', True),
        ('%%GET_NULLABLE_OCTET_COLUMN%%', 'octet', True),
        ('%%GET_NULLABLE_BOOL_COLUMN%%', 'bool', True),
        ('%%GET_NULLABLE_STRING_COLUMN%%', 'string', True),
        ('%%GET_NULLABLE_DATETIME_COLUMN%%', 'datetime', True),
        ('%%GET_NULLABLE_BUFFER_COLUMN%%', 'buffer', True)];
    for placeholder, type_name, is_null in defs:
        impl = replace_get_methods_of_type(impl, columns, placeholder, type_name, is_null)
    return impl

def instantiate_column(columns, name):
    pos = 1
    for column_name, attrs in columns.items():
        if column_name == name:
            base_var_type_enum = get_base_var_type_enum(data_type, column_type)
            return 'column(%s, "%s", %s, %s)' % ('column::' + attrs['type'] + '_e', column_name, pos, 'true' if attrs['nullable'] else 'false')
        pos += 1

    raise Exception("column %s not found" % name)

def replace_init_column_defs(impl, table, columns, indexes):
    cols = []
    if 'PRIMARY' in indexes:
        i = indexes['PRIMARY'][0][0]
        if columns[i]['nullable']:
            raise Exception("nullable primary key found for " + table)
        else:
            ind_column = i
    else:
        raise Exception("no primary key found for " + table)

    pos = 1
    for column_name, attrs in columns.items():
        val = 'column(%s, "%s", %s, %s)' % ('column::' + attrs['type'] + '_e', column_name, pos, 'true' if attrs['nullable'] else 'false')
        cols.append('o_columns.push_back(%s);' % val)
        if column_name == ind_column:
            cols.append('o_primary_key = %s;' % val)
        pos += 1

    return impl.replace('%%INIT_COLUMNS%%', indent('\n'.join(cols), 12))

def replace_init_foreign_keys(impl, table, columns, foreign_keys):
    keys = []
    if table in foreign_keys:
        for referring_table_name, referring_column_name, referenced_column_name, other_columns in foreign_keys[table]:
            keys.append('o_foreign_keys.push_back(foreign_key("%s", %s, "%s", %s));' % (table, instantiate_column(columns, referenced_column_name), referring_table_name, instantiate_column(other_columns, referring_column_name)))
    return impl.replace('%%INIT_FOREIGN_KEYS%%', indent('\n'.join(keys), 12))

def replace_get_member_methods(impl, class_name, columns):
    methods = ''
    for column_name, attrs in columns.items():
        var_type = get_var_type(attrs['type'], attrs['nullable'])
        methods += var_type + ' ' + class_name + '::get_' + column_name + '() const\n'
        methods += '{\n'
        methods += '    return m_' + column_name + ';\n'
        methods += '}\n\n'
    return impl.replace('%%GET_METHODS%%', methods)

def replace_set_member_methods(impl, class_name, columns):
    methods = ''
    for column_name, attrs in columns.items():
        var_type = get_var_type(attrs['type'], attrs['nullable'])
        methods += 'void ' + class_name + '::set_' + column_name + '(' + var_type + ' v)\n'
        methods += '{\n'
        methods += '    m_' + column_name + ' = v;\n'
        methods += '}\n\n'
    return impl.replace('%%SET_METHODS%%', methods)

def get_method_name(attrs, columns):
    method_name = None
    for c in attrs:
        col_name, unique = c
        col_type, col_index = get_col_type_and_index(columns, col_name)
        if method_name is None:
            method_name = col_name
        else:
            method_name += '_' + col_name

    return method_name

def replace_get_by_methods(impl, class_name, columns, indexes):
    methods = ''
    for index_name, attrs in indexes.items():

        validate_index(attrs, columns, index_name, class_name)
        method_name = get_method_name(attrs, columns)
        params = get_method_params(columns, attrs)

        col_name, unique = attrs[0]
        col_type, col_index = get_col_type_and_index(columns, col_name)

        key_col = attrs[0][0]

        if unique:
            methods += 'shared_ptr<' + class_name + '> ' + class_name + '::get_by_' + method_name + '(connection &c, ' + params + ')\n'
            methods += '{\n'
            methods += '    init_static();\n'
            methods += '    return convert_row_type(instance.fetch_row(c, o_columns[' + str(col_index - 1) + '], ' + key_col + '));\n'
            methods += '}\n\n'
        else:
            methods += 'vector<shared_ptr<' + class_name + '>> ' + class_name + '::get_by_' + method_name + '(connection &c, ' + params + ')\n'
            methods += '{\n'
            methods += '    init_static();\n'
            methods += '    return convert_row_type(instance.fetch_rows(c, o_columns[' + str(col_index - 1) + '], ' + key_col + '));\n'
            methods += '}\n\n'
    impl = impl.replace('%%GET_BY_METHODS%%', methods)
    return impl

def replace_delete_by_methods(impl, class_name, columns, indexes):
    methods = ''
    for index_name, attrs in indexes.items():

        validate_index(attrs, columns, index_name, class_name)
        method_name = get_method_name(attrs, columns)
        params = get_method_params(columns, attrs)

        col_name, unique = attrs[0]
        col_type, col_index = get_col_type_and_index(columns, col_name)

        key_col = attrs[0][0]

        methods += 'void ' + class_name + '::delete_by_' + method_name + '(connection &c, ' + params + ')\n'
        methods += '{\n'
        methods += '    init_static();\n'
        methods += '    instance.delete_rows(c, o_columns[' + str(col_index - 1) + '], ' + key_col + ');\n'
        methods += '}\n\n'
    impl = impl.replace('%%DELETE_BY_METHODS%%', methods)
    return impl

def generate_hpp(table, columns, indexes):
    class_name = 'row_' + table
    output_file = class_name + '.hpp'
    header = read_file('row_template_hpp')
    header = replace_basics(header, class_name, table)
    header = replace_member_vars(header, columns)
    header = replace_get_method_decls(header, columns)
    header = replace_set_method_decls(header, columns)
    header = replace_get_by_method_decls(header, class_name, columns, indexes)
    header = replace_delete_by_method_decls(header, class_name, columns, indexes)
    write_file(output_file, header)

def generate_cpp(table, columns, indexes, foreign_keys):
    class_name = 'row_' + table
    output_file = class_name + '.cpp'
    impl = read_file('row_template_cpp')
    impl = replace_basics(impl, class_name, table)
    impl = replace_constructor(impl, columns)
    impl = replace_set_methods(impl, columns)
    impl = replace_get_methods(impl, columns)
    impl = replace_init_column_defs(impl, table, columns, indexes)
    impl = replace_init_foreign_keys(impl, table, columns, foreign_keys)
    impl = replace_get_member_methods(impl, class_name, columns)
    impl = replace_set_member_methods(impl, class_name, columns)
    impl = replace_get_by_methods(impl, class_name, columns, indexes)
    impl = replace_delete_by_methods(impl, class_name, columns, indexes)
    impl = impl.replace('std::', '')
    write_file(output_file, impl)
    
def process_table(name, columns, indexes, foreign_keys):
    print('generating row_%s' % name)
    generate_hpp(name, columns, indexes)
    generate_cpp(name, columns, indexes, foreign_keys)

def gen_table_sql(conf):
    res = ''

    for f in conf:
        js = json.loads(open(f).read())
        for name, body in js.items():
            tsql, ptsql, isql = create_table_sql(name, body, False)
            res += '        o_table_sql[mysql_e].push_back(pair<table_t, string>(' + name + '_e, "' + tsql + '"));\n';
            res += '        o_table_sql[sqlite_e].push_back(pair<table_t, string>(' + name + '_e, "' + tsql + '"));\n';
            res += '        o_table_sql[mongodb_e].push_back(pair<table_t, string>(' + name + '_e, "' + name + '"));\n';
            res += '        o_table_sql[postgres_e].push_back(pair<table_t, string>(' + name + '_e, "' + ptsql + '"));\n';

    return res

def create_mongo_index_params(table, attrs):

    res = []

    for ind_name, ind_col in attrs['indexes'].items():
        col_name, col_unique = ind_col[0]
        res.append({ 'table' : table, 'column' : col_name, 'unique' : col_unique })

    return res


def gen_index_sql(conf):
    res = ''

    for f in conf:
        js = json.loads(open(f).read())
        for name, body in js.items():
            res += '        o_index_sql[mysql_e].push_back(pair<table_t, list<string>>(' + name + '_e, list<string>()));\n'
            res += '        o_index_sql[sqlite_e].push_back(pair<table_t, list<string>>(' + name + '_e, list<string>()));\n'
            res += '        o_index_sql[mongodb_e].push_back(pair<table_t, list<string>>(' + name + '_e, list<string>()));\n'
            res += '        o_index_sql[postgres_e].push_back(pair<table_t, list<string>>(' + name + '_e, list<string>()));\n'

            tsql, ptsql, isql = create_table_sql(name, body, False)
            mongo = create_mongo_index_params(name, body)

            for sql in isql:
                res += '        o_index_sql[mysql_e].back().second.push_back("' + sql + '");\n'
                res += '        o_index_sql[sqlite_e].back().second.push_back("' + sql + '");\n'
                res += '        o_index_sql[postgres_e].back().second.push_back("' + sql + '");\n'

            for params in mongo:
                s = json.dumps(params)
                s = s.replace('"', '\\"')
                res += '        o_index_sql[mongodb_e].back().second.push_back("' + s + '");\n'

    return res

def gen_fk_sql(conf):
    res = ''

    foreign_keys = {}

    for f in conf:
        js = json.loads(open(f).read())
        for name, body in js.items():
            foreign_keys[name] = body['foreign_keys']

    for table_name, attrs in foreign_keys.items():
        res += '        o_fk_sql[mysql_e].push_back(pair<table_t, list<string>>(' + table_name + '_e, list<string>()));\n'
        res += '        o_fk_sql[sqlite_e].push_back(pair<table_t, list<string>>(' + table_name + '_e, list<string>()));\n'
        res += '        o_fk_sql[postgres_e].push_back(pair<table_t, list<string>>(' + table_name + '_e, list<string>()));\n'

        for other_table_name, col_details in attrs.items():
            sql = 'ALTER TABLE ' + other_table_name + ' ADD FOREIGN KEY (' + col_details['column_name'] + ') REFERENCES ' + table_name + ' (' + col_details['referenced_column_name'] + ')'
            res += '        o_fk_sql[mysql_e].back().second.push_back("' + sql + '");\n'
            res += '        o_fk_sql[postgres_e].back().second.push_back("' + sql + '");\n'

    return res

def process_table_hpp(tables):
    header = read_file('table_template_hpp')
    header = header.replace('%%DATETIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))
    enum = [t + '_e' for t in tables]
    header = header.replace('%%TABLES%%', indent(',\n'.join(enum), 16))
    write_file("table.hpp", header)

def gen_row_includes(conf):
    res = ''
    for f in conf:
        js = json.loads(open(f).read())
        for name, body in js.items():
            res += '#include "row_' + name + '.hpp"\n'
    return res

def gen_create_static(static):
    res = ''
    s = json.loads(open(static).read())

    ids = set()

    # get list of IDs to create
    for table in s:
        name = table['table']
        for col, val in table['values']:
            if col[-3:] == '_id':
                ids.add((col, val))

    for col, val in ids:
        res += '    uuid ' + col + '_' + str(val) + ' = boost::uuids::random_generator()();\n'
                
    for table in s:
        name = table['table']
        values = table['values']
        res += '    {\n'
        res += '        row_' + name + ' _' + name + ';\n'

        for (col_name, col_value) in values:
            if type(col_value) == type(''):
                col_value = '"' + col_value + '"'
            if type(col_value) == type(True):
                col_value = "true" if col_value else "false"
            if col_name[-3:] == '_id':
                res += '        _' + name + '.set_' + col_name + '(' + col_name + '_' + str(col_value) + ');\n'
            elif col_name.find('date_time') != -1:
                res += '        _' + name + '.set_' + col_name + '(datetime(' + str(col_value) + '));\n'
            else:
                res += '        _' + name + '.set_' + col_name + '(' + str(col_value) + ');\n'
        res += '        _' + name + '.insert_row(conn);\n'
        res += '    }\n'
    return res

def process_table_cpp(conf, static):
    impl = read_file('table_template_cpp')
    impl = impl.replace('%%DATETIME%%', datetime.datetime.now().strftime('%I:%M%p on %B %d, %Y'))
    impl = impl.replace('%%TABLESQL%%', gen_table_sql(conf));
    impl = impl.replace('%%INDEXSQL%%', gen_index_sql(conf));
    impl = impl.replace('%%FKSQL%%', gen_fk_sql(conf));
    impl = impl.replace('%%ROWINCLUDES%%', gen_row_includes(conf));
    impl = impl.replace('%%STATICDATA%%', gen_create_static(static));
    write_file("table.cpp", impl)

def create_row_classes(conf, static):
    tables = []
    for f in conf:
        js = json.loads(open(f).read())
        for name, body in js.items():
            print('creating row class for', name)
            tables.append(name)
            if len(body['indexes'].keys()) < 1:
                print('table', name, 'has no indexes defined')
                exit(0)
            if len(body['columns'].keys()) < 1:
                print('table', name, 'has no columns defined')
                exit(0)
            process_table(name, body['columns'], body['indexes'], body['foreign_keys'])

    process_table_hpp(tables)
    process_table_cpp(conf, static)

def sql_col_type(attrs, pk, db="default"):
    type_map = {
        'bool'    : 'BOOLEAN',
        'int'     : 'INTEGER',
        'octet'   : 'INTEGER UNSIGNED',
        'uint'    : 'INTEGER UNSIGNED',
        'uuid'    : 'VARCHAR(36)',
        'datetime': 'DATETIME',
        'string'  : 'VARCHAR(255)'
    }

    if db == "postgres":
        type_map['uint'] = 'INTEGER'
        type_map['octet'] = 'INTEGER'
        type_map['datetime'] = 'TIMESTAMP'

    res = type_map[attrs['type']]

    if attrs['type'] == 'string' and 'length' in attrs:
        res = 'VARCHAR(' + str(attrs['length']) + ')'
        
    if attrs['nullable']:
        res += ' NULL'
    else:
        res += ' NOT NULL'
    if pk:
        res += ' PRIMARY KEY'
    return res

def is_pk(name, indexes):
    return indexes['PRIMARY'][0][0] == name

def create_table_sql(name, attrs, script=True):
    res1 = 'CREATE TABLE ' + name
    if script:
        res1 += '\n(\n'
    else:
        res1 += '('
    
    n = len(attrs['columns'].keys())
    for col_name, col_attrs in attrs['columns'].items():
        if script:
            res1 += '    '
        res1 += col_name + ' ' + sql_col_type(col_attrs, is_pk(col_name, attrs['indexes']))
        n -= 1
        if  n > 0:
            res1 += ', '
            if script:
                res1 += '\n'
    if script:
        res1 += '\n);\n\n'
    else:
        res1 += ')'

    res2 = 'CREATE TABLE ' + name
    if script:
        res2 += '\n(\n'
    else:
        res2 += '('
    
    n = len(attrs['columns'].keys())
    for col_name, col_attrs in attrs['columns'].items():
        if script:
            res2 += '    '
        res2 += col_name + ' ' + sql_col_type(col_attrs, is_pk(col_name, attrs['indexes']), db="postgres")
        n -= 1
        if  n > 0:
            res2 += ', '
            if script:
                res2 += '\n'
    if script:
        res2 += '\n);\n\n'
    else:
        res2 += ')'

    resl = []
    ind = ''
    for ind_name, ind_col in attrs['indexes'].items():
        if ind_name != 'PRIMARY':
            col_name, col_unique = ind_col[0]
            if col_unique:
                ind += 'CREATE UNIQUE INDEX '
            else:
                ind += 'CREATE INDEX '
            ind += ind_name + ' ON ' + name + '('  + col_name
            if script:
                ind += ');\n\n'
            else:
                ind += ')'
            resl.append(ind)

    return res1, res2, resl

if len(sys.argv) != 2:
    print('usage', sys.argv[0], 'config-file (e.g. dbconf.json)')
    exit(1)
else:
    js = json.loads(open(sys.argv[1]).read())
    if 'tables' in js:
        create_row_classes(js['tables'], js['static'])
        exit(0)
    else:
        print("no includes config section found")
        exit(1)
