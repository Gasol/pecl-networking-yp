#!/bin/bash

sudo apt-get update
sudo debconf-set-selections <<< 'nis nis/domain string precise32'
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y expect php5-cli php5-dev nis valgrind
sudo sed -i'' -re 's/NISSERVER=(.*)/NISSERVER=true/' /etc/default/nis
sudo sed -i'' -re 's|(YPSRCDIR = ).*|\1/vagrant/tests/data|' /var/yp/Makefile
sudo sed -i'' -re 's|(YPPWDDIR = ).*|\1/vagrant/tests/data|' /var/yp/Makefile
sudo service ypserv start
sudo expect -c '
spawn /usr/lib/yp/ypinit -m
expect "<control D>"
send "precise32\n"
send ""
expect "Is this correct?"
send "\n"
expect eof
'
