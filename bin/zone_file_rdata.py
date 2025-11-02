import ipaddress
import base64
import zone_file_exception
from zone_file_token import zone_file_token
from zone_file_util import *
from zone_file_exception import zone_file_exception 

NAME         = 1
UINT16       = 2
UINT32       = 3
STRING       = 4
TTL          = 5
IP4          = 6
IP6          = 7
MULTI_BASE64 = 8
INT16        = 9
OCTET        = 10
TEXT         = 11
LOWERALNUM   = 12
BITMAP       = 13
TXT          = 14
EUI48        = 15
EUI64        = 16
SVCPARAMS    = 17
IPSECKEY     = 18
ZERO         = 19
POLAR        = 20
MULTI_HEX    = 21

record_fields = {
    'SOA' : 
        [
            ('mname',   NAME),
            ('rname',   NAME),
            ('serial',  UINT32),
            ('refresh', TTL),
            ('retry',   TTL),
            ('expire',  TTL),
            ('minimum', TTL)
        ],
    'A' : 
        [
            ('ip4_addr', IP4)
        ],
    'AAAA' : 
        [
            ('ip6_addr', IP6)
        ],
    'NS' : 
        [
            ('nsdname', NAME)
        ],
    'MX' : 
        [
            ('preference',  UINT16),
            ('exchange',    NAME)
        ],
    'CNAME' : 
        [
            ('cname', NAME)
        ],
    'DNAME' : 
        [
            ('dname', NAME)
        ],
    'PTR' : 
        [
            ('dname', NAME)
        ],
    'NAPTR' : 
        [
            ('naptr_order',     UINT16),
            ('preference',      UINT16),
            ('flags',           STRING),
            ('services',        STRING),
            ('naptr_regexp',    STRING),
            ('replacement',     NAME)
        ],
    'SRV' : 
        [
            ('priority',    UINT16),
            ('weight',      UINT16),
            ('port',        UINT16),
            ('target',      NAME)
        ],
    'HINFO' : 
        [
            ('cpu', STRING),
            ('os',  STRING)
        ],
    'AFSDB' : 
        [
            ('subtype', INT16),
            ('hostname',  NAME)
        ],
    'CAA' :
        [
            ('flags', OCTET),
            ('tag',  LOWERALNUM),
            ('contents',  TEXT)
        ],
    'CERT' :
        [
            ('certificate_type', UINT16),
            ('key_tag', UINT16),
            ('algorithm', OCTET),
            ('certificate',  MULTI_BASE64)
        ],
    'CSYNC' :
        [
            ('soa_serial', UINT32),
            ('flags', UINT16),
            ('type_bitmap', BITMAP)
        ],
    'DHCID' :
        [
            ('contents',  MULTI_BASE64)
        ],
    'TXT' :
        [
            ('txt',  TXT)
        ],
    'EUI48' :
        [
            ('eui48',  EUI48)
        ],
    'EUI64' :
        [
            ('eui64',  EUI64)
        ],
    'SVCB' :
        [
            ('priority',  UINT16),
            ('target',  NAME),
            ('svcparams',  SVCPARAMS)
        ],
    'HTTPS' :
        [
            ('priority',  UINT16),
            ('target',  NAME),
            ('svcparams',  SVCPARAMS)
        ],
    'IPSECKEY' :
        [
            ('ipseckey',  IPSECKEY)
        ],
    'KX' :
        [
            ('preference',  UINT16),
            ('exchanger',  NAME)
        ],
    'LOC' :
        [
            ('coordinates', POLAR)
        ],
    'OPENPGPKEY' :
        [
            ('pgpkey', MULTI_BASE64)
        ],
    'SSHFP' :
        [
            ('algorithm', OCTET),
            ('fp_type', OCTET),
            ('fingerprint', MULTI_HEX)
        ],
    'TLSA' :
        [
            ('certifcate_usage', OCTET),
            ('selector', OCTET),
            ('matching_type', OCTET),
            ('certificate_association', MULTI_HEX)
        ],
    'URI' :
        [
            ('priority', UINT16),
            ('weight',  UINT16),
            ('target', STRING)
        ],
    'ZONEMD' :
        [
            ('soa_serial', UINT32),
            ('scheme', OCTET),
            ('algorithm', OCTET),
            ('digest', MULTI_HEX)
        ]
}

def form_name(ORIGIN, n):
    if n[-1] == '.':
        return n
    else:
        return n + '.' + ORIGIN

def get_txt(f, tokens):
    res = {'strings' : []}

    for t in tokens:
        if t.type() != zone_file_token.STRING:
            raise zone_file_exception('invalid token type for TXT record (must be string)', f)
        else:
            if len(t.value()) > 255:
                raise zone_file_exception('string greater than 255 characters for TXT record', f)
            res['strings'].append(t.value())

    return res

def get_svcparams(f, tokens):
    res = {}
    for t in tokens:
        if t.value() == 'no-default-alpn':
            res['no-default-alpn'] = 'set'
        else:
            i = t.value().find('=')
            if i == -1:
                raise zone_file_exception("syntax error in svcparam '%s'" % t.value(), f)
            key = t.value()[0:i]
            value = t.value()[i+1:].replace('"', '')
            if key == 'port':
                res[key] = int(value)
            elif key == 'ech':
                res[key] = value
            elif key in ['alpn', 'mandatory', 'ipv4hint', 'ipv6hint']:
                res[key] = value.split(',')
            else:
                raise zone_file_exception("unsupported key in svcparam '%s'" % key, f)
    return res

def get_base64(f, tokens):
    v = ''
    for t in tokens:
        v += t.value()
    if valid_base64(v):
        return v
    else:
        raise zone_file_exception("value '%s' for %s record is invalid base64" % (v, field_type), f)

def get_hex(f, tokens):
    v = ''
    for t in tokens:
        v += t.value()
    if valid_hex(v):
        return v
    else:
        raise zone_file_exception("value '%s' for %s record is invalid hex" % (v, field_type), f)

def get_ipseckey(f, tokens):
    res = {}
    if len(tokens) < 5:
        raise zone_file_exception('malformed ipseckey field', f)

    res['precedence'] = int(tokens[0].value())
    res['algorithm'] = int(tokens[1].value())
    t = res['gateway_type'] = int(tokens[2].value())

    if t == 0:
        pass
    elif t == 1:
        res['ipv4_gateway'] = tokens[3].value()
    elif t == 2:
        res['ipv6_gateway'] = tokens[3].value()
    else:
        res['name_gateway'] = tokens[3].value()

    res['public_key'] = get_base64(f, tokens[4:])

    return res

def polar_range_check(x, l, u):
    if x >= l and x <= u:
        return
    else:
        raise zone_file_exception('malformed polar coordinates', f)

def translate_loc_size(cm):

    if cm < 0:
        raise zone_file_exception('cannot translate negative location size %s' % cm, f)
    elif cm == 0:
        return 0
    else: 
        mult = 0
        n = cm
        while n > 9:
            n = int(n/10)
            mult += 1
            if mult > 9:
                raise zone_file_exception('location size too big %s' % cm, f)
        return n * 16 + mult

def get_polar(f, tokens):
    res = {}

    i = 0 

    lat_dir = ''

    d_lat = 0
    m_lat = 0
    s_lat = 0

    d_lon = 0
    m_lon = 0
    s_lon = 0

    alt = None

    size = '1m'
    horiz_p = '10000m'
    vert_p = '10m'

    d_lat = int(tokens[i].value())
    i+= 1

    if tokens[i].value() not in ['N', 'S']:
        m_lat = int(tokens[i].value())
    else:
        lat_dir = tokens[i].value()
    i+= 1

    if tokens[i].value() not in ['N', 'S']:
        s_lat = int(tokens[i].value())
    else:
        lat_dir = tokens[i].value()
    i+= 1

    if tokens[i].value() not in ['N', 'S']:
        raise zone_file_exception('malformed polar coordinates', f)
    else:
        lat_dir = tokens[i].value()
    i+= 1

    d_lon = int(tokens[i].value())
    i+= 1

    if tokens[i].value() not in ['E', 'W']:
        m_lon = int(tokens[i].value())
    else:
        lon_dir = tokens[i].value()
    i+= 1
    if tokens[i].value() not in ['E', 'W']:
        s_lon = int(tokens[i].value())
    else:
        lon_dir = tokens[i].value()
    i+= 1

    if tokens[i].value() not in ['E', 'W']:
        raise zone_file_exception('malformed polar coordinates', f)
    else:
        lon_dir = tokens[i].value()
    i+= 1
    
    alt = tokens[i].value()
    i+= 1

    if i < len(tokens):
        size = tokens[i].value()
        i+= 1

    if i < len(tokens):
        horiz_p = tokens[i].value()
        i+= 1

    if i < len(tokens):
        vert_p = tokens[i].value()
        i+= 1

    if i < len(tokens):
        if len(tokens) == i + 3:
            size = tokens[i].value()
            horiz_p = tokens[i + 1].value()
            vert_p = tokens[i + 2].value()
        else:
            raise zone_file_exception('malformed polar coordinates', f)

    polar_range_check(d_lon, 0, 90)
    polar_range_check(d_lat, 0, 180)
    polar_range_check(m_lon, 0, 59)
    polar_range_check(m_lat, 0, 59)
    polar_range_check(s_lon, 0, 59.999)
    polar_range_check(s_lat, 0, 59.999)

    lat = int((d_lat * 60 * 60 * 1000) + (m_lat * 60 * 1000) + (s_lat * 1000))
    if lat_dir == 'N':
        lat = 2**31 + lat
    else:
        lat = 2**31 - lat

    lon = int((d_lon * 60 * 60 * 1000) + (m_lon * 60 * 1000) + (s_lon * 1000))
    if lon_dir == 'E':
        lon = 2**31 + lon
    else:
        lon = 2**31 - lon

    alt = 10000000 + int(alt.replace('m', '')) * 100 
    size = int(size.replace('m', ''))
    horiz_p = int(horiz_p.replace('m', ''))
    vert_p = int(vert_p.replace('m', ''))

    res['version'] = 0
    res['size'] = translate_loc_size(size * 100)
    res['horizontal_precision'] = translate_loc_size(horiz_p * 100)
    res['vertical_precision'] = translate_loc_size(vert_p *100)

    res['latitude'] = lat
    res['longitude'] = lon
    res['altitude'] = alt
    return res

def parse_rdata(f, ORIGIN, res, tokens):

    t = res['type']

    if t not in record_fields:
        raise zone_file_exception("unsupported record type '%s'" % t, f)

    for n in range(0, len(record_fields[t])):

        field_name, field_type = record_fields[t][n]

        if field_type == TXT:
            res[field_name] = get_txt(f, tokens[n:])
            return
        elif field_type == SVCPARAMS:
            res[field_name] = get_svcparams(f, tokens[n:])
            return
        elif field_type == ZERO:
            res[field_name] = 0
            n -= 1    
        elif field_type == POLAR:
            coords = get_polar(f, tokens[n:])
            for k, v in coords.items():
                res[k] = v
            return
        elif field_type == IPSECKEY:
            res[field_name] = get_ipseckey(f, tokens[n:])
            return
        elif field_type == NAME:
            name = form_name(ORIGIN, tokens[n].value())
            if valid_fqdn(f, name):
                res[field_name] = name
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid" % (field_name, tokens[n].value(), field_type), f)
        elif field_type == EUI48:
            if valid_eui48(tokens[n].value()):
                res[field_name] = tokens[n].value()
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid" % (field_name, tokens[n].value(), field_type), f)
        elif field_type == EUI64:
            if valid_eui64(tokens[n].value()):
                res[field_name] = tokens[n].value()
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid" % (field_name, tokens[n].value(), field_type), f)
        elif field_type == OCTET:
            if valid_octet(tokens[n].value()):
                res[field_name] = int(tokens[n].value())
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid" % (field_name, tokens[n].value(), field_type), f)
        elif field_type == UINT16:
            if valid_uint16(tokens[n].value()):
                res[field_name] = int(tokens[n].value())
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid" % (field_name, tokens[n].value(), field_type), f)
        elif field_type == INT16:
            if valid_int16(tokens[n].value()):
                res[field_name] = int(tokens[n].value())
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid" % (field_name, tokens[n].value(), field_type), f)
        elif field_type == UINT32:
            if valid_uint32(tokens[n].value()):
                res[field_name] = int(tokens[n].value())
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid" % (field_name, tokens[n].value(), field_type), f)
        elif field_type == STRING:
            if tokens[n].type() == zone_file_token.STRING:
                res[field_name] = tokens[n].value()
            else:
                raise zone_file_exception('invalid token type for STRING in %s record' % t, f)
        elif field_type == TTL:
            res[field_name] = translate_ttl(f, tokens[n].value())
        elif field_type == IP4:
            a = ipaddress.ip_address(tokens[n].value())
            if type(a) is ipaddress.IPv4Address:
                res[field_name] = tokens[n].value()
            else:
                raise zone_file_exception("address '%s' for %s record is invalid" % (tokens[n].value(), field_type), f)
        elif field_type == IP6:
            a = ipaddress.ip_address(tokens[n].value())
            if type(a) is ipaddress.IPv6Address:
                res[field_name] = tokens[n].value()
            else:
                raise zone_file_exception("address '%s' for %s record is invalid" % (tokens[n].value(), field_type), f)
        elif field_type == TEXT:
            res[field_name] = tokens[n].value()
        elif field_type == MULTI_BASE64:
            res[field_name] = get_base64(f, tokens[n:])
            return
        elif field_type == MULTI_HEX:
            res[field_name] = get_hex(f, tokens[n:])
            return
        elif field_type == BITMAP:
            v = []
            for i in range(n, len(tokens)):
                if tokens[i].value() in record_fields:
                    v.append(tokens[i].value())
                else:
                    raise zone_file_exception("record type '%s' for %s record is invalid" % (tokens[i].value(), field_type), f)
            res[field_name] = v
            return
        elif field_type == LOWERALNUM:
            if is_lower_alnum(tokens[n].value()):
                res[field_name] = tokens[n].value()
            else:
                raise zone_file_exception("%s '%s' for %s record is invalid, must be lowercase alnum" % (field_name, tokens[n].value(), t), f)
        else:
            raise zone_file_exception("bad field type definition '%s'" % field_type, f)

    # only get here if there wasn't a multi-field type at the end of the rdata
    # check that all tokens were used and complain if not
    if len(tokens) != len(record_fields[t]):
        raise zone_file_exception("wrong number of rdata fields for record type '%s'" % t, f)

