#!/bin/sh

TMP_ARGS=`echo $1 | sed -e "s/\interface //g"`
TMP_ARGS=`echo $TMP_ARGS | sed -e "s/\ add / /g"`

INTERFACE_NAME=""
BRIDGE_NAME=""

for i in $(echo $TMP_ARGS | tr " ")
do
	if [ "x"$INTERFACE_NAME = "x" ]
	then
		INTERFACE_NAME=$i
		continue
	fi

	if [ "x"$BRIDGE_NAME = "x" ]
	then
		BRIDGE_NAME=$i
	fi
done

/usr/sbin/brctl addif $BRIDGE_NAME $INTERFACE_NAME

if [ $? = "0" ]
then
	echo "Interface "$INTERFACE_NAME" added to bridge "$BRIDGE_NAME
fi

exit 0
