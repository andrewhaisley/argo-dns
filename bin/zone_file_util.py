import base64
import string
import binascii

def translate_ttl(f, s):
    number_part = ''
    while len(s) > 0:
        if str.isdigit(s[0]):
            number_part += s[0]
            s = s[1:]
        else:
            break

    if number_part == '':
        raise zone_file_exception('invalid TTL value', f)

    number_part = int(number_part)

    if s == '':
        pass
    elif s == 'm':
        number_part *= 60
    elif s == 'h':
        number_part *= 60*60
    elif s == 'd':
        number_part *= 60*60*24
    elif s == 'w':
        number_part *= 60*60*24*7
    else:
        raise zone_file_exception('invalid TTL value', f)

    if number_part > (2**32 - 1):
        raise zone_file_exception('TTL max value exceeded', f)
    else:
        return number_part

def valid_ttl(f, s):
    try:
        translate_ttl(f, s)
        return True
    except:
        return False

def valid_octet(s):
    try:
        n = int(s)
        return n <= 255 and n >= 0
    except:
        return False

def valid_int16(s):
    try:
        n = int(s)
        return n <= 32767 and n >= -32768
    except:
        return False

def valid_uint16(s):
    try:
        n = int(s)
        return n <= 65536 and n >= 0
    except:
        return False

def valid_uint32(s):
    try:
        n = int(s)
        return n < 2**32 and n >= 0
    except:
        return False

def valid_fqdn(f, s):
    # must be at least 1 character long
    if s == '' or s is None:
        raise zone_file_exception('invalid empty name at line', f)

    # must be letters, digits, ._', and '-' symbols only
    for c in s:
        if not str.isalnum(c) and c != '-' and c != '.' and c != '_':
            raise zone_file_exception("invalid character '%c' in name" % s, f)
        
    # must end in '.'
    if s[-1] != '.':
        raise zone_file_exception("name '%s' doesn't end in '.'" % s, f)

    # must be no more than 254 characters long including periods
    if len(s) > 254:
        raise zone_file_exception("name '%s' too long '.'" % s, f)

    # must have no more than 128 labels
    if len(s.split('.')) > 128:
        raise zone_file_exception("name '%s' has too many labels '.'" % s, f)

    return True

def is_lower_alnum(s):
    for c in s:
        if not c.islower() or not c.isalnum():
            return False
    return True

def valid_base64(s):
    try:
        base64.b64decode(s)
        return True
    except ValueError:
        return False

def valid_eui48(s):
    if len(s) != 17:
        return False
    bits = s.split('-')
    if len(bits) != 6:
        return False
    for b in bits:
        if len(b) != 2:
            return False
        for c in b:
            if c not in string.hexdigits:
                return False
    return True

def valid_eui64(s):
    if len(s) != 23:
        return False
    bits = s.split('-')
    if len(bits) != 8:
        return False
    for b in bits:
        if len(b) != 2:
            return False
        for c in b:
            if c not in string.hexdigits:
                return False
    return True

def valid_hex(s):
    try:
        binascii.unhexlify(s)
        return True
    except binascii.Error:
        return False
