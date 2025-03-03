#!/usr/bin/python3

from json import dumps, loads

from test_support import *

def test_run_state():

    print("TESTING run API")

    response_code, data = make_request('GET', '1/run', None)
    print("response code is ", response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print("run test 1: FAILED")

def test_monitor():

    print("TESTING monitor API")

    response_code, data = make_request('GET', '1/monitor', None)
    print("response code is ", response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print("monitor test 1: FAILED")

def test_server():

    print("TESTING server API")

    response_code, data = make_request('GET', '1/server', None)
    print("response code is ", response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print("server test 1: FAILED")

def test_horizon():

    print("TESTING horizon API")

    response_code, data = make_request('GET', '1/horizon', None)
    print("response code is ", response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print("horizon test 1: FAILED")

def test_zone():

    print("TESTING zone API")

    response_code, data = make_request('GET', '1/zone', None)
    print("response code is ", response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print("zone test 1: FAILED")


def test():
    test_run_state()
    test_monitor()
    test_server()
    test_horizon()
    test_zone()


test()
