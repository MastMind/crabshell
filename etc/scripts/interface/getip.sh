#!/bin/sh

TMP_ARGS=`echo $1 | sed -e "s/\interface //g"`

INTERFACE_NAME=`echo $TMP_ARGS | sed -e "s/\ dhcp//g"`

/sbin/dhclient $INTERFACE_NAME >/dev/null 2>/dev/null

if [ $? != "0" ]
then
	echo "Ip addr for "$INTERFACE_NAME" is not received by DHCP"
fi

exit 0
