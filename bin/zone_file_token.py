from zone_file_exception import zone_file_exception

class zone_file_token:

    """
    Lexical tokens read from zone files.

    Special characters (those escaped by \ or represented by \DDD) are only allowed 
    in strings. If you want to load unicode domain names, use the REST API.

    Attributes
    ----------
    _type : number
        the type of the token - one of the class constants defined
    _value : string 
        the raw value of the token (e.g. abc.com, "blah", 123 etc)

    Methods
    -------
    read_token(f)
        Reads the next token from the supplied zone file reader
    """
    
    # types of token
    ORIGIN      = 1     # $ORIGIN 
    INCLUDE     = 2     # $INCLUDE
    TTL         = 3     # $TTL
    AT          = 4     # origin - i.e. @
    OPEN        = 5     # (
    CLOSE       = 6     # )
    STRING      = 7     # something enclosed in quotes. e.g "abc"
    WORD        = 8     # any freestanding non quoted piece of text (e.g. numbers & names)
    EOL         = 9     # \n
    COMMENT     = 10    # comment including the trailing EOL

    def __init__(self, p_type, value=None):
        self._type = p_type
        self._value = value

    def value(self):
        return self._value

    def type(self):
        return self._type

    @classmethod
    def consume_whitespace(cls, f):
        while True:
            c = f.read()
            if c != ' ' and c != '\t':
                f.putback(c)
                return

    @classmethod
    def consume_to_eol(cls, f):
        while True:
            c = f.read()
            if c == '\n':
                return

    @classmethod
    def read_string(cls, f):
        res = ''
        while True:
            c = f.read()
            if c == '"':
                return res
            elif c == '\\':
                c = f.read()
                if c in ['\n', '']:
                    raise zone_file_exception('malformed escape in string', f)
                elif str.isdigit(c):
                    c2 = f.read()
                    c3 = f.read()
                    if str.isdigit(c2) and str.isdigit(c3):
                        n = ((ord(c) - '0') * 100)    \
                            + ((ord(c1) - '0') * 10)  \
                            + (ord(c2) - '0')
                        if n > 255:
                            raise zone_file_exception('malformed escape in string - value is > 255', f)
                        else:
                            res += chr(n)
                    else:
                        raise zone_file_exception('malformed escape in string', f)
                else:
                    res += c
            else:
                res += c

    @classmethod
    def read_to_next_token(cls, f):
        res = ''
        while True:
            c = f.read()
            if c == ' ' or c == '\t':
                return res
            elif c == '\n' or c == '(' or c == ')':
                f.putback(c)
                return res
            else:
                res += c

    @classmethod
    def translate_control_entry(cls, f, s):
        if s == 'TTL':
            return zone_file_token(cls.TTL)
        elif s == 'ORIGIN':
            return zone_file_token(cls.ORIGIN)
        elif s == 'INCLUDE':
            return zone_file_token(cls.INCLUDE)
        else:
            raise zone_file_exception('invalid control entry', f, s)

    @classmethod
    def read(cls, f):
        cls.consume_whitespace(f)
        c = f.read()
        if c == '':
            return None
        elif c == ';':
            cls.consume_to_eol(f)
            return zone_file_token(cls.COMMENT)
        elif c == '$':
            return cls.translate_control_entry(f, cls.read_to_next_token(f))
        elif c == '@':
            return zone_file_token(cls.AT, "@")
        elif c == '\n':
            return zone_file_token(cls.EOL)
        elif c == '(':
            return zone_file_token(cls.OPEN)
        elif c == ')':
            return zone_file_token(cls.CLOSE)
        elif c == '"':
            return zone_file_token(cls.STRING, cls.read_string(f))
        elif str.isalnum(c) or c == '\\' or c == '.' or c == '*' or c == '-' or c == '+':
            return zone_file_token(cls.WORD, c + cls.read_to_next_token(f))
        else:
            raise zone_file_exception("invalid character '%s'" % c, f)
