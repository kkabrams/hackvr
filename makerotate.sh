#!/bin/sh
while true;do
 for i in $(seq 0 359);do
  echo "$1 rotate 0 $i 0"
 done
done
