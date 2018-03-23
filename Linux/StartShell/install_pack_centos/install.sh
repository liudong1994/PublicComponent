#!/bin/bash

INSTALL_DIR=/home/MY
PRO_MAIN=myprogram

chmod +x $PRO_MAIN
chmod +x *.sh

# install rc.local (CentOS6)
# cp -f rc.local /etc/rc.d/rc.local
# chmod +x /etc/rc.d/rc.local

# install service  (CentOS7)
cp -f myprogram.service /usr/lib/systemd/system/
systemctl enable myprogram.service

if [ ! -d $INSTALL_DIR ] #未安装
then
	echo "Install myprogram start ..."

	mkdir -p $INSTALL_DIR
	
	cp $PRO_MAIN $INSTALL_DIR/
	cp -rf ./Config $INSTALL_DIR/

else # 已经存在，升级

	echo "Upgrade myprogram start ..."

	PID=`ps -ef | grep $PRO_MAIN$ | grep -v grep | awk '{print $2}'`

	while [ ! -z $PID ]; do
		kill -9 $PID
	    PID=`ps -ef | grep $PRO_MAIN$ | grep -v grep | awk '{print $2}'`
	done

	rm -fr $INSTALL_DIR/$PRO_MAIN
	cp -fr $PRO_MAIN $INSTALL_DIR/
	
fi

cp update.sh $INSTALL_DIR/
cp start_myprogram.sh $INSTALL_DIR/

echo "End ..."


#reboot

