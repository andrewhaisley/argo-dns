#!/usr/bin/python3

from json import dumps, loads
import urllib3
import time 

from test_support import *

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

def create_zone(horizon_id):
    zone = {
                "horizon_id":           horizon_id,
                "name":                 ".",
                "is_forwarded":         True,
                "forward_ip_address":   "8.8.8.8",
                "forward_port":         53
            }

    print("creating zone")
    response_code, data = make_request('POST', '1/zone', zone)
    if response_code != 200:
        print("failed to create zone", data)
        exit(0)

def get_standard_address_list_ids():
    response_code, data = make_request('GET', '1/address_list', None)
    if response_code != 200:
        print('failed to get list of address_lists', data)
        exit(0)

    data = json.loads(data)

    dns_al_id = None
    comm_al_id = None
    
    for al in data['address_lists']:
        if len(al['address_list_addresses']) == 0:
            dns_al_id = al['address_list_id']
        else:
            comm_al_id = al['address_list_id']

    return dns_al_id, comm_al_id


def delete_all_horizons():
    response_code, data = make_request('GET', '1/horizon', None)
    if response_code != 200:
        print('failed to get list of horizons', data)
        exit(1)

    data = json.loads(data)

    for h in data['horizons']:
        print("deleting horizon", h['horizon_id'])
        response_code, data = make_request('DELETE', '1/horizon/' + h['horizon_id'], None)
        if response_code != 200:
            print('failed to delete horizon', h['horizon_id'], data)
            exit(1)

def create_horizon(al_id):
    h = {
        "address_list_id" : al_id,
        "allow_recursion" : False,
        "check_order"     : 0
    }

    response_code, data = make_request('POST', '1/horizon', h)

    if response_code != 200:
        print('failed to create horizon', data)
        return

    return json.loads(data)['horizon_id']


def create_base_data():
    print('creating base horizon & zone data');
    
    # delete all existing horizons (and therefore zones)
    delete_all_horizons()

    # get the standard address lists for DNS requests and for command connections
    dns_al_id, comm_al_id  = get_standard_address_list_ids()
    horizon_id = create_horizon(dns_al_id)
    print('new horizon ID is', horizon_id)
    return horizon_id


def setup():
    horizon_id = create_base_data()
    create_zone(horizon_id)

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
setup()
