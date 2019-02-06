#!/usr/bin/env bash
USAGE="usage: makelabel.sh labelname x y z < labelcontent"
target="$1"
xoffset=$2
yoffset=$3
zoffset=$4
if [ "_$PREFIX" = "_" ];then
 echo '$PREFIX' is not set. I use this to find where my font files are stored. 
 exit 1
fi
if [ "_$1" = "_-h" -o "_$1" = "_--help" -o "_$1" = "_-help" ];then
 echo $USAGE 
 exit 0 #we were asked for help and we succeeded with supplying help. :)
fi
if [ "_" = "_$1" -o "_" = "_$2" -o "_" = "_$3" -o "_" = "_$4" ];then
 echo "something is not right. maybe this will help:" >&2
 echo $USAGE >&2
 exit 1 #something fucked up, but let's tell them how they should have done it.
fi
if grep ' ' <<<$1;then
 echo 'fuck. no spaces in $1.'
 exit 1
fi
##target should not have any spaces or else it fucks shit up. >_>... oh well. hex encode it.
while read -rN 1 c;do
 name="$(printf "%02x" "'$c")"
 if [ "$name" = "0a" ]; then
  xoffset=-6
  yoffset="$(printf '%d - %d\n' $yoffset 10 | bc)"
 fi
 if grep "^$name " ${PREFIX}/share/hackvr/font/default.hackvr 2>&1 > /dev/null;then #don't do this shit unless we actually have something to draw. awk in offsetshape bitches when there's nothing.
   grep "^$name " ${PREFIX}/share/hackvr/font/default.hackvr \
    | sed 's/^'"$name"'/'"$target"'/' \
    | offsetshape.sh "$xoffset" "$yoffset" "$zoffset"
 fi
 xoffset="$(printf '%d + %d\n' $xoffset 6 | bc)"
done
