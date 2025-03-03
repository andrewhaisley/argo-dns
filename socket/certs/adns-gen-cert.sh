#/usr/bin/bash

# Generate a self signed certificate for SSH connections to the control server.
# this isn't the safeest way to do things as certificate verification won't work.
# If you're going to be accessing the server over the public internet then 
# get a certificate from one of the usual providers. You might want to consider
# making that an IP named certificate (e.g. "123.123.123.123") rather than a domain
# names one (e.g. dns.mydomain.com) as it'll be easier to work with in times of 
# no DNS.
openssl req -x509 -newkey rsa:4096 -keyout privkey.pem -out fullchain.pem -sha256 -days 3650 -nodes -subj "/C=XX/ST=StateName/L=CityName/O=CompanyName/OU=CompanySectionName/CN=localhost" -addext "subjectAltName = DNS:localhost"
