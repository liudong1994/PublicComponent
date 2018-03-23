#!/bin/bash

echo "update myprogram"

cd /opt/upgrade/
tar -zxvf install_pack.tar.gz 
cd install_pack/
chmod +x ./install.sh
./install.sh

sleep 3

#reboot

