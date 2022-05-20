#!/bin/sh

INTERFACES_DIR=/sys/class/net
TMP=/tmp/ls.out

cd $INTERFACES_DIR
ls > $TMP
while read line; do
	cd $INTERFACES_DIR/$line
	ls bridge >/dev/null 2>/dev/null
	IS_BRIDGE=$?
	TYPE=`cat type`
	STATE=`cat operstate`
	if [ $IS_BRIDGE != "0" -a $TYPE = "1" ]
	then
		echo "Interface: "$line" ("$STATE")"
	fi
done < $TMP
rm $TMP

exit 0
