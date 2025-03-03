#!/usr/bin/python3

from json import dumps, loads
import urllib3
import auth_test
import time 

from test_support import *

def test_run_state():

    print('TESTING run API')

    response_code, data = make_request('GET', '1/run', None)
    print('response code is', response_code)
    if response_code == 200:
        #print(json.dumps(json.loads(data), indent=4, sort_keys=True))
        pass
    else:
        print('run test 1: FAILED')

def restart_server():

    print('restarting server')

    response_code, data = make_request('PUT', '1/run', { 'run_state' : 'restart' })
    if response_code != 200:
        print('server restart failed')
    else:
        count = 0
        while True:
            try:
                response_code, data = make_request('GET', '1/run', None)
                if response_code == 200:
                    if json.loads(data)['run_state'] == 'run':
                        return
            except requests.exceptions.ConnectionError:
                time.sleep(1)
                
            if ++count > 10:
                print('FAILED TO RESTART SERVER')
                exit(0)

def test_address_list():

    print('TESTING address_list API')

    response_code, data = make_request('GET', '1/address_list', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('run test 1: FAILED')


    al = {}

    al['address_list_id'] = 'e9e10f1b-1808-405d-923c-10d491b0cce3'
    al['address_list_addresses'] = ['193.168.1.14', '193.168.1.15']
    n1 = {}
    n1['address'] = '196.168.0.0/8'
    n2 = {}
    n2['address'] = '196.168.1.0/8'
    al['address_list_networks'] = [n1, n2]

    response_code, data = make_request('PUT', '1/address_list/%s' % al['address_list_id'], al)
    print('response code is', response_code)
    if response_code == 200:
        print('address list test 2: PASSED')
        #print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('address list test 2: FAILED')

    #response_code, data = make_request('DELETE', '1/address_list/4ab25b83-0679-4a90-b3cb-d6411a9d6a51', None)
    #print('response code is', response_code)
    #if response_code == 200:
        #print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    #else:
        #print('run test 1: FAILED', data)

def test_monitor():

    print('TESTING monitor API')

    response_code, data = make_request('GET', '1/monitor', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
        pass
    else:
        print('monitor test 1: FAILED')

def test_server():

    print('TESTING server API')

    response_code, data = make_request('GET', '1/server', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('server test 1: FAILED')

    servers = json.loads(data)
    cs_id = None
    for s in servers['servers']:
        if 'control' in s:
            cs_id = s['server_id']
            break

    cs = {
            "control": {
                "address_list": {
                    "address_list_addresses": [],
                    "address_list_networks": [
                        {
                            "address": "::/0"
                        },
                        {
                            "address": "0.0.0.0/0"
                        }
                    ]
                },
                "allowed_connection_backlog": 10,
                "client_connection_timeout_ms": 30000,
                "maximum_payload_size": 1000000,
                "num_threads": 5,
                "use_ssl": True
            },
            "protocol": "control",
            "server_id": cs_id,
            "socket_addresses": [
                {
                    "ip_address": "0.0.0.0",
                    "port": 2000
                },
                {
                    "ip_address": "::",
                    "port": 2000
                }
            ],
            "transport": "tcp"
        }

    response_code, data = make_request('PUT', '1/server/' + cs['server_id'], cs)
    print('response code is', response_code)
    if response_code == 200:
        print('server test 1: PASSED')
    else:
        print('server test 1: FAILED')

    response_code, data = make_request('GET', '1/server', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('server test 1: FAILED')


def test_base_config():

    print('TESTING base_configuration API')

    response_code, data = make_request('GET', '1/base_configuration', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('base configuration test 1: FAILED')

    m = {
        "edns_size": 5000,
        "support_edns": False,
        "use_ip4_root_hints": False,
        "use_ip6_root_hints":True 
    }

    response_code, data = make_request('PUT', '1/base_configuration', m)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('base configuration test 2: FAILED')

    response_code, data = make_request('GET', '1/base_configuration', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('base configuration test 3: FAILED')

    m = {
        "edns_size": 4096,
        "support_edns": True,
        "use_ip4_root_hints": True,
        "use_ip6_root_hints": False
    }

    response_code, data = make_request('PUT', '1/base_configuration', m)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('base configuration test 4: FAILED')

def test_cache_config():

    print('TESTING cache API')

    response_code, data = make_request('GET', '1/cache_configuration', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('cache configuration test 1: FAILED')

    m = {
        "cache_garbage_collect_ms": 60001,
        "cache_max_answer_rrs": 10001,
        "cache_max_referral_rrs": 10001,
        "client": {
            "connect_tcp_timeout_ms": 1001,
            "num_parallel_udp": 3,
            "read_tcp_timeout_ms": 1001,
            "server_port": 53,
            "total_timeout_ms": 5001,
            "udp_timeout_ms": 2501,
            "use_ip4": True,
            "use_ip6": False,
            "use_tcp": True,
            "use_udp": True,
            "wait_udp_response_ms": 1001,
            "write_tcp_timeout_ms": 1001
        },
        "default_ttl": 300
    }

    response_code, data = make_request('PUT', '1/cache_configuration', m)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('cache configuration test 2: FAILED')

    response_code, data = make_request('GET', '1/cache_configuration', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('cache configuration test 3: FAILED')


def test_horizon():

    print('TESTING horizon API')

    response_code, data = make_request('GET', '1/horizon', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('horizon test 1: FAILED')

    return 
    # insert new horizon
    h = {}
    h['address_list_id'] = '4ab25b83-0679-4a90-b3cb-d6411a9d6a51'
    h['allow_recursion'] = False
    response_code, data = make_request('POST', '1/horizon', h)

    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('horizon test 2: FAILED')


def test_zone():

    print('TESTING zone API')

    response_code, data = make_request('GET', '1/zone', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('zone test 1: FAILED')

    return 


def test_rr(horizon_id):

    print('TESTING rr API')

    nz = {
            'authority': {
                'ns': [
                    {
                        'name': 'ppp.com',
                        'nsdname': 'ns1.ppp.com',
                        'ttl': 604800,
                        'type': 'NS'
                    },
                    {
                        'name': 'ppp.com',
                        'nsdname': 'ns2.ppp.com',
                        'ttl': 604800,
                        'type': 'NS'
                    }
                ],
                'soa': {
                    'expire': 2419200,
                    'minimum': 604800,
                    'mname': 'ppp.com',
                    'name': 'ppp.com',
                    'refresh': 604800,
                    'retry': 86400,
                    'rname': 'admin.ppp.com',
                    'serial': 2,
                    'ttl': 604800,
                    'type': 'SOA'
                }
            },
            'horizon_id': horizon_id,
            'is_forwarded': False,
            'name': 'ppp.com',
            'resource_records': []
        }

    response_code, data = make_request('POST', '1/zone', nz)

    if response_code != 200:
        print('rr test, create test zone : FAILED')
        print(data)
        return

    z = json.loads(data)
    stime = time.time()


    https_rr = {    "name":     "xyz.ppp.com",
                    "ttl":      604800,
                    "type":     "HTTPS",
                    "zone_id":  z['zone_id'],
                    "priority": 1,
                    "target":   "",
                    "svcparams": { 1 : "h2h3" }
            }

    response_code, data = make_request('POST', '1/rr?zone=%s' % z['zone_id'], https_rr)
    if response_code != 200:
        print('rr test, create HTTPS record : FAILED')

    response_code, data = make_request('GET', '1/zone', None)
    print('response code is', response_code)
    if response_code == 200:
        print(json.dumps(json.loads(data), indent=4, sort_keys=True))
    else:
        print('zone test 1: FAILED')


    return

    if False:
        for i in range(0, 1000):

            a_rr = { "ip4_addr": "2.1.%s.%s" % (int(i/256), int(i%256)),
                     "name": "t%s.ppp.com" % i,
                     "ttl": 604800,
                     "type": "A",
                     "zone_id": z['zone_id']
                    }

            response_code, data = make_request('POST', '1/rr?zone=%s' % z['zone_id'], a_rr)
            if response_code != 200:
                print('rr test, create A record : FAILED')
    else:
        payload = []
        for i in range(0, 100):

            a_rr = { "ip4_addr": "2.1.%s.%s" % (int(i/256), int(i%256)),
                     "name": "t%s.ppp.com" % i,
                     "ttl": 604800,
                     "type": "A",
                     "zone_id": z['zone_id']
                    }

            payload.append(a_rr)

        response_code, data = make_request('POST', '1/rr?zone=%s' % z['zone_id'], payload)
        if response_code != 200:
            print('rr test, create A records : FAILED')

    etime = time.time()
    print("created 100 records in", etime-stime, "seconds");

def delete_all_horizons():

    response_code, data = make_request('GET', '1/horizon', None)
    if response_code != 200:
        print('failed to get list of horizons', data)
        return

    data = json.loads(data)

    for h in data['horizons']:
        print("deleting horizon", h['horizon_id'])
        response_code, data = make_request('DELETE', '1/horizon/' + h['horizon_id'], None)
        if response_code != 200:
            print('failed to delete horizon', h['horizon_id'], data)
            return

def get_standard_address_list_ids():

    response_code, data = make_request('GET', '1/address_list', None)
    if response_code != 200:
        print('failed to get list of address_lists', data)
        return

    data = json.loads(data)
    #print(data, indent=4, sort_keys=True))

    dns_al_id = None
    comm_al_id = None
    
    for al in data['address_lists']:
        if len(al['address_list_addresses']) == 0:
            dns_al_id = al['address_list_id']
        else:
            comm_al_id = al['address_list_id']

    return dns_al_id, comm_al_id
    
def create_horizon(al_id):
    h = {
        "address_list_id"    : al_id,
        "allow_recursion" : False,
        "check_order"     : 0
    }

    response_code, data = make_request('POST', '1/horizon', h)

    if response_code != 200:
        print('failed to create horizon', data)
        return

    return json.loads(data)['horizon_id']

def create_recursive_horizon(al_id):
    h = {
        "address_list_id"    : al_id,
        "allow_recursion" : True,
        "check_order"     : 0
    }

    response_code, data = make_request('POST', '1/horizon', h)

    if response_code != 200:
        print('failed to create recursive horizon', data)
        return

def create_zones(horizon_id):
    zones = json.loads(open('test_zones.json').read())

    for z in zones:
        z['horizon_id'] = horizon_id
        print("creating zone")
        response_code, data = make_request('POST', '1/zone', z)
        if response_code != 200:
            print("failed to create zone", data)

def create_base_data():

    print('creating base horizon & zone data');
    
    # delete all existing horizons (and therefore zones)
    delete_all_horizons()

    # get the standard address lists for DNS requests and for command connections
    dns_al_id, comm_al_id  = get_standard_address_list_ids()

    horizon_id = create_horizon(dns_al_id)

    print('new horizon ID is', horizon_id)

    create_zones(horizon_id)

    return horizon_id

def enable_recursion():
    delete_all_horizons()
    # get the standard address lists for DNS requests and for command connections
    dns_al_id, comm_al_id  = get_standard_address_list_ids()
    create_recursive_horizon(dns_al_id)

def test_auth():    
    print("testing authoritative server")
    if not auth_test.run_rr_tests():
        print("AUTHORITATIVE SERVER TESTS FAILED")

def test():
    #enable_recursion()
    horizon_id = create_base_data()
    #restart_server()
    #test_auth()
    #test_run_state()
    #test_monitor()
    #test_server()
    #test_base_config()
    #test_cache_config()
    #test_address_list()
    #test_horizon()
    #test_zone()
    #test_rr(horizon_id)

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
test()

#while True:
    #os.system('dig @localhost -p 1053 +tcp ppp.com SOA')

