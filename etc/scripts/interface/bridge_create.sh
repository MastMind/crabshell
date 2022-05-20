#!/bin/sh

BRIDGE_NAME=`echo $1 | sed -e "s/\create bridge //g"`
/usr/sbin/brctl addbr $BRIDGE_NAME

if [ $? = "0" ]
then
	echo "Bridge "$BRIDGE_NAME" created"
fi

exit 0
