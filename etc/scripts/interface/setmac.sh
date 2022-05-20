#!/bin/sh

TMP_ARGS=`echo $1 | sed -e "s/\interface //g"`
TMP_ARGS=`echo $TMP_ARGS | sed -e "s/\ mac random//g"`

INTERFACE_NAME=""
MAC=""

for i in $(echo $TMP_ARGS | tr " ")
do
	if [ "x"$INTERFACE_NAME = "x" ]
	then
		INTERFACE_NAME=$i
		continue
	fi

	if [ "x"$MAC = "x" ]
	then
		MAC=$i
	fi
done

/usr/bin/macchanger -m $MAC $INTERFACE_NAME

if [ $? != "0" ]
then
	echo "Can't change MAC for "$INTERFACE_NAME
fi

exit 0
