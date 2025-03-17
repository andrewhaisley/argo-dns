# argo-dns
The Argo DNS Server. A high performance, scalable DNS server with database storage
and a REST API. 
# features
- Authoritative DNS
- Recursive DNS
- DNS Forwarding
- DNS over UDP/TCP
- DNS over TLS
- DNS over HTTPS
- REST API for dynamic configuration
- Database back-end
- Reads BIND Zone files
- ipv4/ipv6 support
# supported DBs
- SQLite 
- Mysql
- Postgres
- MongoDB
# supported record types
- A
- AAAA
- AFSDB
- CAA
- CERT
- CNAME
- CSYNC
- DHCID
- DNAME
- EUI48
- EUI64
- HINFO
- SVCB
- HTTPS
- IPSECKEY
- KX
- LOC
- MX
- NAPTR
- NS
- OPENPGPKEY
- PTR
- SOA
- SRV
- SSHFP
- SVCB
- TLSA
- TXT
- URI
# building on Ubuntu

## Install the prerequisites
```
sudo apt install git
sudo apt install g++
sudo apt install cmake
sudo apt install bind9-dnsutils
sudo apt install libssldev
sudo apt install libboost-dev
sudo apt install libsqlite3-dev
sudo apt install libpq-dev
sudo apt install libbson-dev
sudo apt install libmongoc-dev
sudo apt install libmysqlcppconn-dev
```
## Clone the repo
```
git clone https://github.com/andrewhaisley/argo-dns.git
```
## Compile
```
cmake .
make
```
# Running the server
First...

## Create an SSL Certificate
This must be created before the server will start up. A script is provided to create a self-signed certificate for development purposes.
```
cd socket/certs
./adns-gen-cert.sh
```
Optionally, you can install the root certificate into your system's standard locations for such things allowing local development connections
without warning messages.
```
./adns-install-cert.sh
```
## Start the Server
A default configuration file is provided that starts a server using a SQLITE3 database. The DB is created by the server on first startup.
```
./adns --config-file=config.jsn
```
The server default configuration uses the following ports:
- 1053 - DNS over UDP/TCP
- 1443 - DNS over HTTPS
- 1853 - DNS over TLS
- 2000 - management REST API
## Resolve Something
```
dig @127.0.0.1 -p 1053 github.com
```

