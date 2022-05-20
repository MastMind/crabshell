#!/bin/sh

TMP_ARGS=`echo $1 | sed -e "s/\interface //g"`

INTERFACE_NAME=`echo $TMP_ARGS | sed -e "s/\ mac random//g"`

/usr/bin/macchanger -r $INTERFACE_NAME

if [ $? != "0" ]
then
	echo "Can't change MAC for "$INTERFACE_NAME
fi

exit 0
