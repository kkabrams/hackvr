#!/bin/bash
target="$1"
if grep ' ' <<<$1;then
 echo 'fuck. no spaces in $1.'
 exit 1
fi
##target should not have any spaces or else it fucks shit up. >_>... oh well. hex encode it.
xoffset=$2
yoffset=$3
zoffset=$4
if [ "_" = "_$xoffset" -o "_" = "_$yoffset" -o "_" = "_$zoffset" ];then
 echo "missing an offset argument! wtf?" >&2
 exit 1
fi
while read -rN 1 c;do
 name="$(printf "%02x" "'$c")"
 if [ "$name" = "0a" ]; then
  xoffset=-6
  yoffset="$(printf '%d - %d\n' $yoffset 10 | bc)"
 fi
 if grep "^$name " font/font.hackvr 2>&1 > /dev/null;then #don't do this shit unless we actually have something to draw. awk in offsetshape bitches when there's nothing.
   grep "^$name " font/font.hackvr \
    | sed 's/^'"$name"'/'"$target"'/' \
    | ./tools/offsetshape.sh "$xoffset" "$yoffset" "$zoffset"
 fi
 xoffset="$(printf '%d + %d\n' $xoffset 6 | bc)"
done
