#!/bin/sh
part1=""
echo "sleeping 3 seconds. might have gnuchess less likely to not do anything." >&2
sleep 3
echo "protover 2"
while read derp command target extra;do
  if [ "_$command" = "_action" ];then
    if printf "%s\n" "$target" | grep -v "^piece_" 2>&1 > /dev/null;then #we are ignoring clicks on pieces in favor of squares atm. when full 3d probably want pieces.
      if [ "_$part1" = "_" ];then
        part1=$target
      else
        #we have two parts. let's tell gnuchess!
        printf "%s%s\n" $part1 $target
        part1=""
      fi
    fi
  fi
done
