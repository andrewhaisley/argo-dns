#!/usr/bin/python3

import requests
import json
import os

verify = False

default_server = 'https://localhost:2000/'

def read_file(name):
    with open(name, 'r') as content_file:
        return content_file.read()

def make_request(method, path, payload, server=default_server):
    try:
        auth = ('admin', 'admin')

        if method == 'GET':
            r = requests.get(server + path, auth=auth, verify=verify)
            return r.status_code, r.text
        elif method == 'PUT':
            r = requests.put(server + path, auth=auth, json=payload, verify=verify)
            return r.status_code, r.text
        elif method == 'POST':
            r = requests.post(server + path, auth=auth, json=payload, verify=verify)
            return r.status_code, r.text
        elif method == 'DELETE':
            r = requests.delete(server + path, auth=auth, verify=verify)
            return r.status_code, r.text
        else:
            print('unsupported method: ', method)

    except IOError as e: 
        if hasattr(e, 'code'): # HTTPError 
            data = e.read()
            return (e.code, json.dumps(json.loads(data), sort_keys=True, indent=4, separators=(',', ': ')))
        elif hasattr(e, 'reason'): # URLError 
            print("can't connect, reason: ", e.reason)
            raise
        else: 
            raise

def sort_json_vectors(json):
    if type(json) == dict:
        for v in json.values():
            if type(v) == list:
                sort_json_vectors(v)
    elif type(json) == list:
        try:
            json.sort()
        except TypeError:
            # likely has dicts in it which can't be compared
            return
        for v in json:
            sort_json_vectors(v)

def json_equal(json1, json2):
    one = json.loads(json1)
    two = json.loads(json2)
    sort_json_vectors(one)
    sort_json_vectors(two)
    one = json.dumps(one, sort_keys=True)
    two = json.dumps(two, sort_keys=True)
    return one == two

def format_json(data):
    return json.dumps(json.loads(data), sort_keys=True, indent=4, separators=(',', ': '))

def print_json_diff(a, b):
    f1 = open("a.json", "w")
    f2 = open("b.json", "w")
    f1.write(format_json(a))
    f2.write(format_json(b))
    f1.close()
    f2.close()
    print(os.system('diff a.json b.json'))
