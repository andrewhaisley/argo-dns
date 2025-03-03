import sys
import os


message = open('COPYRIGHT').read()

def addcp(fn, comment):
    f = open(fn)
    contents = f.read()
    f.close()
    new_contents = ''
    for line in message.split('\n'):
        new_contents += comment + line + '\n'
    new_contents += contents
    nf = open(fn + '.tmp', 'w')
    nf.write(new_contents)
    nf.close()
    os.rename(fn + '.tmp', fn)


for fn in sys.argv:
    if fn != 'addcp.py':
        if fn[-3] == '.py':
            addcp(fn, '# ')
        elif fn[-4:] in ['.cpp', '.hpp', '_cpp', '_hpp']:
            print(fn)
            addcp(fn, '// ')
