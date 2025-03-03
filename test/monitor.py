#!/usr/bin/python3

import sys
from json import dumps, loads
from test_support import *

def check(server=None):

    if server is None:
        response_code, data = make_request('GET', '1/monitor', None)
    else:
        response_code, data = make_request('GET', '1/monitor', None, server=server)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
        #x = list(json.loads(data)['cache']['referral_counts'].items())
        #x.sort(key=lambda x: x[1])
        #for i in range(0, len(x)):
            #print(x[i])
    else:
        print("monitor call failed")

if len(sys.argv) > 1:
    check(sys.argv[1])
else:
    check(None)
