#!/bin/bash

sleep 1

# modify rp_filter
RP_FILTER_PATH=/proc/sys/net/ipv4/conf
if [ -e $RP_FILTER_PATH/all/rp_filter ]; then
	echo 0 > $RP_FILTER_PATH/all/rp_filter
fi

if [ -e $RP_FILTER_PATH/default/rp_filter ]; then
	echo 0 > $RP_FILTER_PATH/default/rp_filter
fi

if [ -e $RP_FILTER_PATH/eth0/rp_filter ]; then
	echo 0 > $RP_FILTER_PATH/eth0/rp_filter
fi

if [ -e $RP_FILTER_PATH/eth1/rp_filter ]; then
	echo 0 > $RP_FILTER_PATH/eth1/rp_filter
fi

sleep 5

# Pro And Screen Parm
PRO_PATH=/home/MY/				# 不能和其他重复 检测程序是否运行时会检测错误
PRO_NAME=myprogram
SCREEN_NAME=screen_myprogram

cd $PRO_PATH


# 创建后台screen并执行命令函数 
# 第一个参数为后台screen名称  第二个参数为要执行的命令 \n结尾
function funcScreenExec()
{
	# 清除一下无效screen 防止被kill掉处于dead状态
	screen -wipe
	
	ScreenPid=`ps -ef | grep ${1} | grep -v grep | awk '{print $2}'`
	if [ "" == "${ScreenPid}" ];then
		date >> tinfo.log  
		echo "---------------- screen -dmS ${1}----------------" >> tinfo.log
		
		# 没有找到SCREEN_NAME的screen 重新创建一个 -dmS后台运行的screen
		screen -dmS ${1}
	fi
	
	date >> tinfo.log
	echo "---------------- ${2} start ----------------" >> tinfo.log
	# 给SCREEN_NAME的screen发送命令 执行命令
	screen -x -S ${1} -p 0 -X stuff ${2}
}


while true ; do

	# Check myprogramPid
	myprogramPid=`ps -ef | grep ${PRO_NAME} | grep -v grep | awk '{print $2}'`
	if [ "" == "${myprogramPid}" ]; then
		funcScreenExec ${SCREEN_NAME} "${PRO_PATH}/${PRO_NAME}\n"
	fi
	
	
	# Memory check
	MemFree=`cat /proc/meminfo | grep MemFree | awk '{print $2}'`
	Buffers=`cat /proc/meminfo | grep Buffers | awk '{print $2}'`
	Cached=`cat /proc/meminfo | grep ^Cached | awk '{print $2}'`

	FreeMem=`expr $MemFree + $Buffers`
	FreeMem=`expr $FreeMem + $Cached`

	if [ $FreeMem -lt 102400 ]; then
		date >> tinfo.log
		echo "----------- Memory less than 100M , $PRO_NAME restart -----------" >> tinfo.log

		while [ "" != "${myprogramPid}" ]
		do
			kill -9 ${myprogramPid}
			myprogramPid=`ps -ef | grep $PRO_NAME$ | grep -v grep | awk '{print $2}'`
		done
	fi

	sleep 5

done

