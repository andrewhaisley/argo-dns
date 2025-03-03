#!/usr/bin/python3

import os
import sys
from collections import Counter
from test_support import *

#
# test by comparing dig results from bind9
#
# authoritative resolution:
#    auth test cases:
#
#        request for zone that exists:
#
#            at an auth level in the tree where there's an SOA and NS records with glue records
#            at an auth level in the tree where there's an SOA and NS records with no glue records:
#            at an auth level in the tree where there's no SOA and no NS records:
#            at an auth level in the tree where there's no SOA but there are delegating NS records:
#
#        request for zone that doesn't exist:
#
#            at an auth level in the tree where there's an SOA and NS records with no glue records:
#            at an auth level in the tree where there's an SOA and NS records with glue records:
#            at an auth level in the tree where there's no SOA and no NS records:
#            at an auth level in the tree where there's no SOA but there are delegating NS records:
#

rr_tests = [\
    'abcd.com a',
    'Abcd.com a',
    'AbCd.com a',
    'ABCD.com a',
    'blah.com any',
    'blah.com a',
    'blah.com aaaa',
    'blah.com ns',
    'blah.com soa',
    'blah.com hinfo',
    'PASS',
    'PASS',
    'PASS',
    'PASS',
    '_ftp._tcp.blah.com srv',
    'PASS',
    'blah1.com any',
    'blah1.com a',
    'blah1.com aaaa',
    'blah1.com ns',
    'blah1.com soa',
    'none.blah.com any',
    'none.blah.com a',
    'none.blah.com aaaa',
    'none.blah.com ns',
    'none.blah.com soa',
    'ns1.blah.com any',
    'ns1.blah.com a',
    'ns1.blah.com aaaa',
    'ns1.blah.com ns',
    'ns1.blah.com soa',
    'x.x.blah.com a',
    'x.x.blah.com aaaa',
    'x.x.blah.com any',
    'x.x.blah.com ns',
    'x.x.blah.com soa',
    'delegated.blah.com a',
    'delegated.blah.com aaaa',
    'delegated.blah.com any',
    'delegated.blah.com ns',
    'delegated.blah.com soa',
    'x.delegated.blah.com a',
    'x.delegated.blah.com aaaa',
    'x.delegated.blah.com any',
    'x.delegated.blah.com ns',
    'x.delegated.blah.com soa',
    'random.com a',
    'random.com aaaa',
    'random.com any',
    'random.com ns',
    'random.com soa',
    'ftp.blah.com a',
    'ftp.blah.com aaaa',
    'ftp.blah.com any',
    'ftp.blah.com ns',
    'ftp.blah.com soa',
    'a.blah.com a',
    'a.blah.com aaaa',
    'a.blah.com any',
    'a.blah.com ns',
    'a.blah.com soa',
    'www.blah.com a',
    'www.blah.com aaaa',
    'www.blah.com any',
    'www.blah.com ns',
    'www.blah.com soa',
    'somewhere.else.com a',
    'somewhere.else.com aaaa',
    'somewhere.else.com any',
    'somewhere.else.com ns',
    'somewhere.else.com soa',
    'www1.blah.com a',
    'www1.blah.com aaaa',
    'www1.blah.com any',
    'www1.blah.com ns',
    'www1.blah.com soa',
    'cname1.blah.com a',
    'cname1.blah.com aaaa',
    'cname1.blah.com any',
    'cname1.blah.com ns',
    'cname1.blah.com soa',
    'hanging-cname.blah.com a',
    'hanging-cname.blah.com aaaa',
    'hanging-cname.blah.com any',
    'hanging-cname.blah.com ns',
    'hanging-cname.blah.com soa',
    'outzone-dname.blah.com a',
    'outzone-dname.blah.com aaaa',
    'outzone-dname.blah.com any',
    'outzone-dname.blah.com ns',
    'outzone-dname.blah.com soa',
    'x.outzone-dname.blah.com a',
    'x.outzone-dname.blah.com aaaa',
    'x.outzone-dname.blah.com any',
    'x.outzone-dname.blah.com ns',
    'x.outzone-dname.blah.com soa',
    'inzone-dname.blah.com a',
    'inzone-dname.blah.com aaaa',
    'inzone-dname.blah.com any',
    'inzone-dname.blah.com ns',
    'inzone-dname.blah.com soa',
    'x.inzone-dname.blah.com a',
    'x.inzone-dname.blah.com aaaa',
    'x.inzone-dname.blah.com any',
    'x.inzone-dname.blah.com ns',
    'x.inzone-dname.blah.com soa',
    'x.inzone2-dname.blah.com a',
    'x.inzone2-dname.blah.com aaaa',
    'x.inzone2-dname.blah.com any',
    'x.inzone2-dname.blah.com ns',
    'x.inzone2-dname.blah.com soa',
    'x.dname1.blah.com a',
    'x.dname1.blah.com aaaa',
    'x.dname1.blah.com any',
    'x.dname1.blah.com ns',
    'x.dname1.blah.com soa',
    'sdf.sdf.kjh.wildcard.blah.com a',
    'sdf.sdf.kjh.wildcard.blah.com aaaa',
    'sdf.sdf.kjh.wildcard.blah.com any',
    'sdf.sdf.kjh.wildcard.blah.com ns',
    'sdf.sdf.kjh.wildcard.blah.com soa',
    'sdf.sdf.kjh.wildcard2.blah.com a',
    'sdf.sdf.kjh.wildcard2.blah.com aaaa',
    'sdf.sdf.kjh.wildcard2.blah.com any',
    'sdf.sdf.kjh.wildcard2.blah.com ns',
    'sdf.sdf.kjh.wildcard2.blah.com soa'
]

zone_transfer_tests = [
    'blah.com axfr',
    'blah.com ixfr=1',
    'blah.com ixfr=100'
]

adns = 'dig @127.0.0.1 -p 1053 '

#bind = 'dig @192.168.0.75 '
#bind = 'dig @192.168.0.20'
#adns = 'dig @192.168.0.20 -p 1053 '

results_dir = 'resolver-results'
known_good_dir = 'resolver-known-good'

def get_line(lines, tok):

    for l in lines:
        if l.find(tok) != -1:
            return l.strip()

    return ''

def get_next_line(lines, tok):

    found = False
    for l in lines:
        if found:
            return l.strip()
        if l.find(tok) != -1:
            found = True

    return ''

def check_response_flags(bind_res, adns_res):

    # ;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 60768
    # ;; flags: qr aa rd; QUERY: 1, ANSWER: 1, AUTHORITY: 2, ADDITIONAL: 3
    # ;; WARNING: recursion requested but not available

    bind_header = get_line(bind_res, '->>HEADER<<-')
    adns_header = get_line(adns_res, '->>HEADER<<-')

    bind_op_code = bind_header.split(' ')[3].strip(',')
    bind_status = bind_header.split(' ')[4].strip(',')

    adns_op_code = adns_header.split(' ')[3].strip(',')
    adns_status = adns_header.split(' ')[4].strip(',')

    bind_flags = get_line(bind_res, ';; flags')
    adns_flags = get_line(adns_res, ';; flags')

    bind_recursion = get_line(bind_res, ';; WARNING: recursion')
    adns_recursion = get_line(adns_res, ';; WARNING: recursion')

    res = ''

    if bind_op_code != adns_op_code:
        res += 'OPCODES differ (%s, %s)' % (bind_op_code, adns_op_code)

    if bind_status != adns_status:
        res += 'STATUSes differ (%s, %s)' % (bind_status, adns_status)

    if bind_flags != adns_flags:
        res += 'FLAGS differ (%s, %s)' % (bind_flags, adns_flags)

    if bind_recursion != adns_recursion:
        res += 'RECURSION notes differ (%s, %s)' % (bind_recursion, adns_recursion)

    return res

def check_edns(bind_res, adns_res):

    bind_edns = get_line(bind_res, 'EDNS:')
    adns_edns = get_line(adns_res, 'EDNS:')

    if bind_edns == adns_edns:
        return ''
    else:
        return "EDNS flags differ:" + bind_edns + ":" + adns_edns

def check_question(bind_res, adns_res):

    bind_question = get_next_line(bind_res, ';; QUESTION SECTION:')
    adns_question = get_next_line(adns_res, ';; QUESTION SECTION:')

    if bind_question == adns_question:
        return ''
    else:
        return "Questions differ:" + bind_question + ":" + adns_question

def get_section_lines(lines, tok):
    
    start = False
    res = []

    for l in lines:
        l = l.strip()
        if start:
            if l == '':
                return res
            else:
                res.append(l)
        else:
            if l.find(tok) != -1:
                start = True

    return sorted(res)

def check_answer(bind_res, adns_res):

    bind_lines = get_section_lines(bind_res, ';; ANSWER SECTION')
    adns_lines = get_section_lines(adns_res, ';; ANSWER SECTION')

    if len(set(bind_lines).difference(set(adns_lines))) == 0 and len(set(adns_lines).difference(set(bind_lines))) == 0:
        return ''
    else:
        return "answers are different"


def check_authority(bind_res, adns_res):

    bind_lines = get_section_lines(bind_res, ';; AUTHORITY SECTION')
    adns_lines = get_section_lines(adns_res, ';; AUTHORITY SECTION')

    if len(set(bind_lines).difference(set(adns_lines))) == 0 and len(set(adns_lines).difference(set(bind_lines))) == 0:
        return ''
    else:
        return "authority records are different"

def check_additional(bind_res, adns_res):

    bind_lines = get_section_lines(bind_res, ';; ADDITIONAL SECTION')
    adns_lines = get_section_lines(adns_res, ';; ADDITIONAL SECTION')

    if len(set(bind_lines).difference(set(adns_lines))) == 0 and len(set(adns_lines).difference(set(bind_lines))) == 0:
        return ''
    else:
        return "additional records are different"


def diff_results(n, test, bind_res_file, adns_res_file):

    with open(bind_res_file) as f:
        bind_res = f.readlines()

    with open(adns_res_file) as f:
        adns_res = f.readlines()

    results = []

    results.append(('flags',       check_response_flags(bind_res, adns_res)))
    results.append(('edns',        check_edns(bind_res, adns_res)))
    results.append(('question',    check_question(bind_res, adns_res)))
    results.append(('answer',      check_answer(bind_res, adns_res)))
    results.append(('authority',   check_authority(bind_res, adns_res)))
    results.append(('additional',  check_additional(bind_res, adns_res)))

    notes = []

    for (test_name, result) in results:
        if result != '':
            notes.append(result)

    if len(notes) == 0:
        print("%s:RR TEST:" % n, "PASSED")
    else:
        print("%s:RR TEST:" % n, test, "FAILED")
        print("   ", notes)

def run_rr_tests():

    n = 0

    for t in rr_tests:

        if t != 'PASS':
            bind_res_file = known_good_dir + '/test-%s-bind-rr-result.txt' % n

            adns_res_file = results_dir + '/test-%s-adns-rr-result.txt' % n
            command = adns + ' ' + t + '>' + adns_res_file
            os.system(command)

            diff_results(n, t, bind_res_file, adns_res_file)

        n += 1

def run_zone_transfer_tests():

    n = 0

    for t in zone_transfer_tests:

        bind_res_file = known_good_dir + '/test-%s-bind-transfer-result.txt' % n

        adns_res_file = results_dir + '/test-%s-adns-transfer-result.txt' % n
        command = adns + ' ' + t + " | grep -v ';' | sort > " + adns_res_file
        os.system(command)

        with open(bind_res_file) as f:
            bind_res = f.readlines()

        with open(adns_res_file) as f:
            adns_res = f.readlines()

        if bind_res == adns_res:
            print("%s:TRANSFER TEST:" % n, "PASSED")
        else:
            print("%s:TRANSFER TEST:" % n, "FAILED")

        n += 1

os.system('rm ' + results_dir + '/*.txt')
#build_test_schema()
#restart_server()
run_rr_tests()
#run_zone_transfer_tests()
