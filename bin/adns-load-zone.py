#!/usr/bin/python3

import sys
import argparse
import json
import requests
import urllib3
import zone_file_parser

hostname = None
port = None
username = None
password = None
verify = None
ssl = None

def read_config(filename):

    config = json.loads(open(filename).read())
    return config['static-config']['server-config-username'], config['static-config']['server-config-password']


def get_args():

    #
    # arguments we need to have:
    #
    #   either:
    #       config_file - path of the server config file
    #
    #   or:
    #       hostname of the DNS server (could be IP address)
    #       port     - control server port for the server
    #       username - admin username
    #       password - admin password
    #
    #   and:
    #       filename - path of the zone file we're going to load
    #   

    parser = argparse.ArgumentParser()

    parser.add_argument('-c', '--config',     help='ADNS configuration file to use')
    parser.add_argument('-u', '--username',   help='ADNS server admin username (if config file not given)')
    parser.add_argument('-p', '--password',   help='ADNS server admin password (if config file not given)')
    parser.add_argument('-H', '--hostname',   help='ADNS server hostname (default is localhost)', default='localhost')
    parser.add_argument('-P', '--port',       help='ADNS server control port (default is 2000)', type=int, default=2000)
    parser.add_argument('-n', '--no-verify',  help="don't verify ssl connection to server", action='store_true')
    parser.add_argument('-s', '--no-ssl',     help="don't use SSL for connection to server", action='store_true')
    parser.add_argument(       '--horizon',   help='server horizon ID for the zone (blank for first horizon)')
    parser.add_argument('filename',           help='zone file to load')

    args = parser.parse_args()

    if args.config:
        username, password = read_config(args.config)
    else:
        if args.username and args.password:
            username = args.username
            password = args.password
        else:
            print('usage: username and password required when not supplying a configuration file.')
            exit(1)

    return (args.hostname, args.port, username, password, args.horizon, not args.no_verify, not args.no_ssl, args.filename)


def make_request(method, path, payload):

    global hostname, port, username, password, verify, ssl

    if not verify:
        urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

    try:
        auth = (username, password)
        if ssl:
            server = 'https://%s:%s/' % (hostname, port)
        else:
            server = 'http://%s:%s/' % (hostname, port)

        if method == 'GET':
            r = requests.get(server + path, auth=auth, verify=verify)
            return r.status_code, json.loads(r.text)
        elif method == 'PUT':
            r = requests.put(server + path, auth=auth, json=payload, verify=verify)
            return r.status_code, json.loads(r.text)
        elif method == 'POST':
            r = requests.post(server + path, auth=auth, json=payload, verify=verify)
            return r.status_code, json.loads(r.text)
        elif method == 'DELETE':
            r = requests.delete(server + path, auth=auth, verify=verify)
            return r.status_code, json.loads(r.text)
        else:
            raise Exception('unsupported method: ', method)
    except IOError as e: 
        if hasattr(e, 'code'): # HTTPError 
            data = e.read()
            return (e.code, json.loads(data))
        else:
            raise


def get_first_horizon_id():

    response_code, data = make_request('GET', '1/horizon', None)
    if response_code == 200:
        if len(data['horizons']) > 0:
            # find the horizon with the lowest check order
            horizons = {}
            for h in data['horizons']:
                horizons[h['check_order']] = h
            min_check = sorted(list(horizons.keys()))[0]
            return horizons[min_check]['horizon_id']
        else:
            raise Exception('no horizons found')
    else:
        raise Exception('failed to read horizons', response_code)

def zone_exists(name):
    response_code, data = make_request('GET', '1/zone?name=%s' % name[:-1].replace('.', '\.'), None)
    if response_code == 200:
        if len(data['zones']) == 0:
            return False, None
        else:
            return True, data['zones'][0]['zone_id']
    else:
        raise Exception('failed to check zone existence', response_code)

def load_zone_file(p_hostname, p_port, p_username, p_password, horizon, p_verify, p_ssl, filename):

    global hostname, port, username, password, verify, ssl

    hostname = p_hostname
    port = p_port
    username = p_username
    password = p_password 
    verify = p_verify
    ssl = p_ssl

    if not horizon:
        horizon = get_first_horizon_id()

    print('Loading zone to horizon ID', horizon)

    rrs = zone_file_parser.get_resource_records(filename)

    if len(rrs) == 0:
        raise Exception('No valid resource records in zone file')

    if rrs[0]['type'] != 'SOA':
        raise Exception('First resource record in zone file is not SOA')

    filtered_rrs = []
    SOA = None
    NS = []

    # get the SOA and domain NS records and remove them from the full list
    for r in rrs:
        if r['type'] == 'SOA':
            if SOA is not None:
                raise Exception('More than one SOA record found')
            else:
                SOA = r
        elif r['type'] == 'NS' and r['name'] == SOA['name']:
            NS.append(r)
        else:
            filtered_rrs.append(r)

    z =  {
            'horizon_id': horizon,
            'is_forwarded': False,
            'name': SOA['name'],
            'authority' : {
                'soa' : SOA,
                'ns' : NS
            }, 
            'resource_records': filtered_rrs
    }

    e, i = zone_exists(SOA['name'])

    if e:
        z['zone_id'] = i
        response_code, data = make_request('PUT', '1/zone/' + i, z)
        if response_code == 200:
            print("Zone updated")
        else:
            print("Zone update failed, response was", response_code, data)
    else:
        response_code, data = make_request('POST', '1/zone', z)
        if response_code == 200:
            print("Zone created")
        else:
            print("Zone create failed, response was", response_code, data)
    

load_zone_file(*get_args())
