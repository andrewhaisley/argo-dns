from zone_file_exception import zone_file_exception
from zone_file_token import zone_file_token 
from zone_file_rdata import parse_rdata
from zone_file_util import translate_ttl, valid_ttl, valid_fqdn

class zone_file_statement:

    # types of statement
    ORIGIN      = 1     # $ORIGIN 
    INCLUDE     = 2     # $INCLUDE
    TTL         = 3     # $TTL
    RECORD      = 4     # resource record
    COMMENT     = 5

    # record types supported
    supported_types = { 'SOA', 'A', 'AAAA', 'NS', 'MX', 'CNAME', 'DNAME', 'PTR', 'TXT', 'NAPTR', 'SRV', 'HINFO',
                        'AFSDB', 'CAA', 'CERT', 'CSYNC', 'DHCID', 'EUI48', 'EUI64', 'SVCB', 'HTTPS', 'IPSECKEY',
                        'KX', 'LOC', 'OPENPGPKEY', 'SSHFP', 'SVCB', 'TLSA', 'URI', 'ZONEMD' }

    def __init__(self, stype=None):
        self._type = stype
        self._origin = None
        self._ttl = None
        self._filename = None
        self._record = None

    def type(self):
        return self._type

    def origin(self):
        return self._origin

    def ttl(self):
        return self._ttl

    def filename(self):
        return self._filename

    def record(self):
        return self._record

    @classmethod 
    def get_name(cls, f, ORIGIN, name):
        if len(name) == 0 or name is None:
            raise zone_file_exception('invalid name', f)
        if name == '@':
            if ORIGIN is None:
                raise zone_file_exception('no default ORIGIN set', f)
            return ORIGIN
        elif name[-1] == '.':
            return name
        else:
            if ORIGIN is None:
                raise zone_file_exception('no default ORIGIN set', f)
            return name + '.' + ORIGIN 

    @classmethod
    def find_record_type(cls, f, tokens):
        for i in range(0, len(tokens)):
            t = tokens[i]
            if t.type() == zone_file_token.WORD:
                if t.value() in cls.supported_types:
                    return (t.value(), i)

        raise zone_file_exception('invalid resource record, no type found', f)

    @classmethod
    def assemble_record(cls, f, tokens, ORIGIN, TTL, last_name, last_ttl):

        # get the default name & ttl to use if these aren't
        # specified in the record. 

        # if there was a name used in a previous record, then default to that
        # otherwise default to the ORIGIN
        if last_name is None:
            last_name = ORIGIN

        # if a default TTL is specified, use that otherwise use the one 
        # from the previous record.
        if TTL is not None:
            last_ttl = TTL

        #
        # records look like one of these:
        #
        # [<owner>] [<TTL>] [<class>] <type> <RDATA>
        # [<owner>] [<class>] [<TTL>] <type> <RDATA>
        #
        # type is mandatory so start by finding that
        #
        res = {'name' : None, 'type' : None, 'ttl' : None}

        (t, i) = cls.find_record_type(f, tokens)

        # must be at least one token to the right of the type for there to be rdata present
        if i == len(tokens) - 1:
            raise zone_file_exception('missing rdata', f)

        res['type'] = t

        # no items to the left of the type, so default the name
        # and TTL (class is always IN)
        if i == 0:
            if last_name is None:
                raise zone_file_exception('unable to default name', f)
            if last_ttl is None:
                raise zone_file_exception('unable to default TTL', f)
            res['name'] = last_name
            res['ttl'] = last_ttl

        # with only one item to the left, assume it's a name. The RFC doesn't
        # define the behaviour here so just do what bind9 does.
        elif i == 1:
            if last_name is None:
                raise zone_file_exception('unable to default name', f)
            if last_ttl is None:
                raise zone_file_exception('unable to default TTL', f)
            res['name'] = cls.get_name(f, ORIGIN, tokens[0].value())
            res['ttl'] = last_ttl

        # with two items to the left there are four valid combinations:
        # TTL IN    - name stays the same as previous record
        # IN TTL    - name stays the same as previous record
        # NAME IN   - TTL stays same or defaults to $TTL
        # NAME TTL  - type defaults to IN
        elif i == 2:
            left = tokens[0]
            right = tokens[1]

            if left.type() != zone_file_token.WORD or left.type() != zone_file_token.WORD:
                raise zone_file_exception('invalid token', f)

            left_is_TTL = valid_ttl(f, left.value())
            right_is_TTL = valid_ttl(f, right.value())
            left_is_IN = left.value() == 'IN'
            right_is_IN = right.value() == 'IN'
            left_is_name = not left_is_TTL and not left_is_IN
            right_is_name = not right_is_TTL and not right_is_IN

            if left_is_TTL and right_is_IN:
                res['ttl'] = translate_ttl(f, left.value())
                res['name'] = last_name
            elif left_is_IN and right_is_TTL:
                res['ttl'] = translate_ttl(f, right.value())
                res['name'] = last_name
            elif left_is_name and right_is_IN:
                res['name'] = cls.get_name(f, ORIGIN, left.value())
            elif left_is_name and right_is_TTL:
                res['name'] = cls.get_name(f, ORIGIN, left.value())
                res['ttl'] = translate_ttl(f, right.value())
            else:
                raise zone_file_exception('invalid resource record', f)

        # with three items to the left we have name, class, and TTL
        # the first is a label, TTL is the one that's a number, the other must be IN
        elif i == 3:
            res['name'] = cls.get_name(f, ORIGIN, tokens[0].value())
            if tokens[1].value() == 'IN':
                res['ttl'] = translate_ttl(f, tokens[2].value())
            elif tokens[2].value() == 'IN':
                res['ttl'] = translate_ttl(f, tokens[1].value())
            else:
                raise zone_file_exception('invalid resource record', f)

        # parse the record type specific data
        parse_rdata(f, ORIGIN, res, tokens[i+1:])

        return res

    @classmethod
    def parse_record(cls, f, t, ORIGIN, TTL, last_name, last_ttl):
        res = zone_file_statement(cls.RECORD)
        tokens = []
        if t.type() != zone_file_token.EOL:
            tokens.append(t)
        in_brackets = False
        while True:
            t = zone_file_token.read(f)
            if t is None or t.type() == zone_file_token.EOL:
                if not in_brackets:
                    break
            elif t.type() == zone_file_token.CLOSE:
                if in_brackets:
                    in_brackets = False
                else:
                    raise zone_file_exception('unmatched closing brace', f)
            elif t.type() == zone_file_token.OPEN:
                if in_brackets:
                    raise zone_file_exception('illegal nested opening brace', f)
                else:
                    in_brackets = True
            elif t.type() == zone_file_token.AT:
                tokens.append(t)
            elif t.type() == zone_file_token.STRING:
                tokens.append(t)
            elif t.type() == zone_file_token.WORD:
                tokens.append(t)
            elif t.type() == zone_file_token.COMMENT:
                if not in_brackets:
                    break
            else:
                raise zone_file_exception('unexpected token (%s,%s)' % (t.type(), t.value()), f)

        if len(tokens) == 0:
            res._record = None
        else:
            res._record = cls.assemble_record(f, tokens, ORIGIN, TTL, last_name, last_ttl)

        return res

    @classmethod
    def read(cls, f, ORIGIN, TTL, last_name, last_ttl):

        t = zone_file_token.read(f)

        if t == None:
            return None

        elif t.type() == zone_file_token.TTL:
            res = zone_file_statement(cls.TTL)
            v = zone_file_token.read(f)
            if v.type() == zone_file_token.WORD:
                res._ttl = translate_ttl(f, v.value())
            else:
                raise zone_file_exception('malformed TTL statement', f)
            return res

        elif t.type() == zone_file_token.ORIGIN:
            res = zone_file_statement(cls.ORIGIN)
            v = zone_file_token.read(f)
            if v.type() == zone_file_token.WORD:
                if valid_fqdn(f, v.value()):
                    res._origin = v.value()
                else:
                    raise zone_file_exception('malformed ORIGIN', f)
            else:
                raise zone_file_exception('malformed ORIGIN statement', f)
            return res

        elif t.type() == zone_file_token.INCLUDE:
            res = zone_file_statement(cls.INCLUDE)
            v = zone_file_token.read(f)
            if v.type() == zone_file_token.STRING:
                res._filename = v.value()
            else:
                raise zone_file_exception('malformed INCLUDE statement', f)
            return res

        elif t.type() == zone_file_token.EOL or t.type() == zone_file_token.COMMENT:
            return zone_file_statement(cls.COMMENT)

        else:
            return cls.parse_record(f, t, ORIGIN, TTL, last_name, last_ttl)

