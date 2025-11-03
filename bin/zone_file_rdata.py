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

def encode_angle(a, nsew):
    seconds =  a * 3600.0
    thousandths = int(round(seconds * 1000.0))
    if nsew in ['n', 'e']:
        val = (1 << 31) + thousandths       
    else:
        val = (1 << 31) - thousandths       
    return val

def extract_angle(f, tokens, i, dirs):
    deg = float(tokens[i].value())
    i += 1

    if tokens[i].value() in dirs:
        return (i + 1, tokens[i].value().lower(), deg)
    else:
        deg += float(tokens[i].value()) / 60
        i += 1

    if tokens[i].value() in dirs:
        return (i + 1, tokens[i].value().lower(), deg)
    else:
        deg += float(tokens[i].value()) / 3600
        i += 1

    if tokens[i].value() in dirs:
        return (i + 1, tokens[i].value().lower(), deg)
    else:
        raise zone_file_exception('missing N|S|E|W in LOC record', f)
        
def encode_loc_size(f, size_meters):

    size_cm = size_meters * 100.0

    # determine mantissa+exponent such that mantissa is 1–9
    exponent = 0
    mantissa = size_cm

    # scale until mantissa fits range 1–9
    while mantissa > 9:
        mantissa /= 10.0
        exponent += 1

    # round mantissa to nearest whole number
    mantissa = round(mantissa)

    # if rounding pushed it out of range, rescale again
    if mantissa == 10:
        mantissa = 1
        exponent += 1

    if not (1 <= mantissa <= 9 and 0 <= exponent <= 9):
        raise zone_file_exception('value cannot be encoded in LOC size format', f)

    return (mantissa << 4) | exponent


def get_polar(f, tokens):

    res = {}

    #
    # 51 36 19 N 71 06 18 W -24m 30m
    # fields:
    #
    #  alt:    [-100000.00 .. 42849672.95] BY .01 (altitude in meters)
    #  siz, hp, vp: [0 .. 90000000.00] (size/precision in meters)       - size defaults to 1m, horiz precision defaults to 10000m, vert precision defaults to 10m
    #

    i = 0

    i, ns, latitude = extract_angle(f, tokens, i, ['N', 'S', 'n', 's'])
    i, ew, longitude = extract_angle(f, tokens, i, ['E', 'W', 'e', 'w'])

    altitude = float(tokens[i].value().replace('m', '').replace('M', '')) * 100.0

    i += 1

    size = float(tokens[i].value().replace('m', '').replace('M', ''))

    i += 1

    if i < len(tokens):
        hp = float(tokens[i].value().replace('m', '').replace('M', '')) 
        i += 1
    else:
        hp = 10000

    if i < len(tokens):
        vp = float(tokens[i].value().replace('m', '').replace('M', '')) 
    else:
        vp = 10

    res['version'] = 0
    res['size'] = encode_loc_size(f, size)
    res['horizontal_precision'] = encode_loc_size(f, hp)
    res['vertical_precision'] = encode_loc_size(f, vp)

    res['latitude'] = encode_angle(latitude, ns)
    res['longitude'] = encode_angle(longitude, ew)
    res['altitude'] = round(altitude) + 10000000

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

