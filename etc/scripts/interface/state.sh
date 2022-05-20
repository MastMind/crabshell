#!/bin/sh

TMP_ARGS=`echo $1 | sed -e "s/\interface //g"`
TMP_ARGS=`echo $TMP_ARGS | sed -e "s/\ set / /g"`

INTERFACE_NAME=""
STATE=""

for i in $(echo $TMP_ARGS | tr " ")
do
	if [ "x"$INTERFACE_NAME = "x" ]
	then
		INTERFACE_NAME=$i
		continue
	fi

	if [ "x"$STATE = "x" ]
	then
		STATE=$i
	fi
done

if [ $STATE = "down" ]
then
	/sbin/dhclient -r $INTERFACE_NAME
fi

/sbin/ip link set dev $INTERFACE_NAME $STATE

if [ $? != "0" ]
then
	echo $INTERFACE_NAME" is not bringed "$STATE
fi

exit 0
