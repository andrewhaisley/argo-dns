#!/usr/bin/python3

import requests
import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

#r = requests.get("https://8.8.8.8/dns-query?dns=AAABAAABAAAAAAAAA3d3dwdleGFtcGxlA2NvbQAAAQAB")
r = requests.get("https://localhost:1443/dns-query?dns=AAABAAABAAAAAAAAA3d3dwdleGFtcGxlA2NvbQAAAQAB")

print(r.status_code)
print(r.text)

for k,v in r.headers.items():
    print(k, ":", v)

