#!/usr/bin/python3

import os
import sys
import csv
import subprocess

bind = ('localhost', '2053')
adns = ('localhost', '1053')

def extract_status(s):

    i = s.find('status: ')
    if i == -1:
        return 'n/a'
    else:
        return s[i+8:s.find(',', i)]

def dig(server, domain):

    args = []

    args.append('dig')
    args.append('+retry=4')
    args.append('@'+server[0])
    args.append('-p')
    args.append(server[1])

    args.append(domain)
    args.append('a')

    out = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    stdout, stderr = out.communicate()

    if out.returncode == 0:
        return out.returncode, stdout, extract_status(stdout)
    else:
        return out.returncode, 'dig failed %s %s %s %s' % (server, domain, stdout, stderr), 'n/a'

def answers(s):
    records = set()
    lines = s.split('\n')
    n = 0
    in_section = False
    while n < len(lines):
        if in_section:
            if lines[n].strip() == '':
                return records
            else:
                x = lines[n].replace('\t', ' ').replace('  ', ' ')
                # remove the ttl
                i = x.find(' ')
                j = x.find(' ', i + 1)
                x = x[0:i] + x[j:]
                records.add(x)
        else:
            if lines[n].find(';; ANSWER SECTION') == 0:
                in_section = True
        n += 1

    return records


def run_tests():
    o = csv.writer(open('results.csv', 'w'))
    c = csv.reader(open('majestic_million.csv'))
    for l in c:
        if l[0] == 'GlobalRank':
            continue

        d = l[2]

        e1, s1, sc1 = dig(bind, d)
        e2, s2, sc2 = dig(adns, d)

        if e1 == 0 and e2 == 0:
            if sc1 == sc2:
                a1 = answers(s1)
                a2 = answers(s2)
                if a1 != a2:
                    if (len(a1) != len(a2)) and (len(a2) == 0):
                        print("====== DOMAIN ", d)
                        print("bind has answers and adns does not,", d)
                        print("------ BIND -------")
                        print(s1)
                        print("------ ARGO -------")
                        print(s2)
                        #for r in a1:
                            #print("  BIND  :", r)
                        #for r in a2:
                            #print("  ARGO  :", r)
                        o.writerow([d, 'adns returns no answer records vs. bind which does'])
            else:
                print("====== DOMAIN ", d)
                print("different status codes")
                print("BIND:", sc1)
                print("ARGO:", sc2)
        else:
            if e1 != e2:
                print("====== DOMAIN ", d)
                print("different exit codes,", e1, e2)
                if e2 != 0:
                    o.writerow([d, 'adns failed and bind succeeded', e1, e2])
                

run_tests()
