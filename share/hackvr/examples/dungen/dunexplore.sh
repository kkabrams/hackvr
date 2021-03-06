#!/usr/bin/env bash
### run me with hackvr_coproc
#the $USER triangle we add first is to give us some sort of avatar to see where the camera is.
#seed=1337
red=1
green=2
blue=4
echo $USER addshape 6 3  -1 0 -1  0 0 2  1 0 -1 ; echo $USER move 0 2 0
echo world-x addshape $red 2  0 0 0  1 0 0
echo world-y addshape $green 2  0 0 0  0 1 0
echo world-z addshape $blue 2  0 0 0  0 0 1
./dungen $seed | ./dun2hackvr
while read group action target;do
  if [ "$action" = "action" ];then
    if printf "%s\n" "${target}" | grep ^door_;then
      printf "# clicked a door! %s\n" "${target}" >&2
      if printf "%s\n" "${target}" | grep ^door_open;then
        tmp=$(printf "%s\n" "${target}" | sed 's/_open/_CLOSING/')
        printf "%s renamegroup %s %s\n" ${target} ${target} ${tmp} 
        (
        for i in $(seq 1 5 90);do
          printf "%s rotate 0 +5 0\n" ${tmp}
          sleep .1
        done
        tmp2=$(printf "%s\n" "${target}" | sed 's/_open/_close/')
        printf "%s renamegroup %s %s\n" ${tmp} ${tmp} ${tmp2}
        ) &
      elif printf "%s\n" "${target}" | grep ^door_close;then
        tmp=$(printf "%s\n" "${target}" | sed 's/_close/_OPENING/')
        printf "%s renamegroup %s %s\n" ${target} ${target} ${tmp}
        (
        for i in $(seq 1 5 90);do
          printf "%s rotate 0 +-5 0\n" ${tmp}
          sleep .1
        done
        tmp2=$(printf "%s\n" "${target}" | sed 's/_close/_open/')
        printf "%s renamegroup %s %s\n" ${tmp} ${tmp} ${tmp2}
        ) &
      fi
    fi 
  fi
done
