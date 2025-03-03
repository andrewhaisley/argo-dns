class zone_file_exception(Exception):

    def __init__(self, error, reader=None, context=None):
        if reader is None:
            super().__init__(self, error, context)
        else:
            super().__init__(self, error + ' at line %s.' % reader.line(), context)
