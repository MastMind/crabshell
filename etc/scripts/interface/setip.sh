#!/bin/sh

TMP_ARGS=`echo $1 | sed -e "s/\interface //g"`
TMP_ARGS=`echo $TMP_ARGS | sed -e "s/\ ip / /g"`
TMP_ARGS=`echo $TMP_ARGS | sed -e "s/\ mask / /g"`

INTERFACE_NAME=""
IP=""
NETMASK=""

for i in $(echo $TMP_ARGS | tr " ")
do
	if [ "x"$INTERFACE_NAME = "x" ]
	then
		INTERFACE_NAME=$i
		continue
	fi

	if [ "x"$IP = "x" ]
	then
		IP=$i
		continue
	fi

	if [ "x"$NETMASK = "x" ]
	then
		NETMASK=$i
	fi
done

#release dhcp
/sbin/dhclient -r $INTERFACE_NAME >/dev/null 2>/dev/null

#flush old addresses
/sbin/ip addr flush dev $INTERFACE_NAME

#set new ip
/sbin/ip addr add $IP/$NETMASK dev $INTERFACE_NAME

if [ $? != "0" ]
then
	echo "Ip addr for "$INTERFACE_NAME" is not setted"
fi

exit 0
