#!/bin/sh

BRIDGE_NAME=`echo $1 | sed -e "s/\delete bridge //g"`
/usr/sbin/brctl delbr $BRIDGE_NAME

if [ $? = "0" ]
then
	echo "Bridge "$BRIDGE_NAME" deleted"
fi

exit 0
