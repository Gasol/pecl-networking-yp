#!/bin/bash

if [ -z "$TRAVIS_BUILD_DIR" ]; then
	YPSRCDIR="/vagrant/tests/data"
else
	YPSRCDIR="$TRAVIS_BUILD_DIR/tests/data"
fi

apt-get update
debconf-set-selections <<< 'nis nis/domain string precise32'
DEBIAN_FRONTEND=noninteractive apt-get install -y expect php5-cli php5-dev nis valgrind
sed -i -re 's/NISSERVER=(.*)/NISSERVER=true/' /etc/default/nis
sed -i -re "s|(YPSRCDIR = ).*|\\1$YPSRCDIR|" /var/yp/Makefile
sed -i -re "s|(YPPWDDIR = ).*|\\1$YPSRCDIR|" /var/yp/Makefile
service ypserv start
expect -c '
spawn /usr/lib/yp/ypinit -m
expect "<control D>"
send "precise32\n"
send ""
expect "Is this correct?"
send "\n"
expect eof
'
