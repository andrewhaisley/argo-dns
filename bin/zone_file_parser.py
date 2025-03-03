from zone_file_exception import zone_file_exception
from zone_file_reader import zone_file_reader
from zone_file_token import zone_file_token
from zone_file_statement import zone_file_statement

def get_resource_records(filename, ORIGIN=None, TTL=None):

    result = []

    f = zone_file_reader(open(filename))

    last_name = None
    last_ttl = None

    while True:
        s = zone_file_statement.read(f, ORIGIN, TTL, last_name, last_ttl)
        if s == None:
            return result
        elif s.type() == s.ORIGIN:
            ORIGIN = s.origin()
        elif s.type() == s.INCLUDE:
            result += get_resource_records(s.filename(), ORIGIN, TTL)
        elif s.type() == s.TTL:
            TTL = s.ttl()
        elif s.type() == s.COMMENT:
            pass
        elif s.type() == s.RECORD:
            if s.record() != None:
                result.append(s.record())
                last_name = s.record()['name']
                last_ttl = s.record()['ttl']
        else:
            raise zone_file_exception('undefined statement type', f)

    return result
