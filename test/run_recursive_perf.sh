#!/bin/bash
#dnsperf -t 25 -p 1053 -d recursive-dnsperf1.txt -l 600000000 -c 100
dnsperf -s haisley.com -t 25 -p 53 -d recursive-dnsperf1.txt -l 600 -c 100
