class zone_file_reader():
    """
    A simple proxy for file.read(1) that keeps track of line numbers,
    allows single characater putback, and discards superflous \r 
    characters from Windows/DOS files.

    Attributes
    ----------
    _file : file
        the file to be read from
    _name : number
        the current line number (starting from 1)
    _putback : character
        the last character putback (if any)

    Methods
    -------
    read()
        Reads a single character from the file keeping track of the line number.
    putback(c)
        Puts a single character back into the file keeping track of the line number.
    line()
        Returns the current line number being read from the file.
    """

    def __init__(self, f):
        self._file = f
        self._line = 1
        self._putback = None

    def read(self):
        if self._putback == None:
            c = self._file.read(1)
            if c == '\n':
                self._line += 1
                return c
            elif c == '\r':       # ignore Windows nastiness
                return self.read()
            else:
                return c
        else:
            c = self._putback
            if c == '\n':
                self._line += 1
            self._putback = None
            return c

    def putback(self, c):
        self._putback = c
        if c == '\n':
            self._line -= 1

    def line(self):
        return str(self._line)
