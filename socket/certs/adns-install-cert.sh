#/usr/bin/bash

# On ubuntu this will add the certificate to the local list of trusted ones so you can make
# ssl connections without warnings.
 sudo cp fullchain.pem /usr/share/ca-certificates/local.crt
 sudo update-ca-certificates

# For things that use openssl
sudo cp fullchain.pem /etc/ssl/certs
