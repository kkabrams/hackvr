#!/bin/bash
turn=$( expr $RANDOM % 2)
echo "go. player: $turn"
stdbuf -oL uniq \
  | grep --line-buffered action \
  | stdbuf -oL cut '-d ' -f1,3 | while read user group;do
    if grep "_reset" <<<$group >/dev/null;then
     printf "%s deleteallexcept .\n" "$user"
     cat board
     continue
    elif grep "_" <<<$group >/dev/null;then
#     xmessage "CUT IT OUT"
     echo CUT IT OUT > /dev/stderr
     continue
    else
     turn=$(expr \( $turn + 1 \) % 2)
     #printf "# turn: %d\n" "$turn" > /dev/stderr
     printf "$user deletegroup %s\n" "$group"
     #printf "$user deletegroup %s\n" "$group" > /dev/stderr
#need to get the first point of the group clicked and translate the new shape by that much
     translatex="$(grep "$group" board | grep -v '^#' | tr -s ' ' | cut '-d ' -f4)"
     translatey="$(grep "$group" board | grep -v '^#' | tr -s ' ' | cut '-d ' -f5)"
     cat "marker$turn"
     printf "%s move %s %s 2\n" "_marker" "$(expr "$translatex")" "$(expr "$translatey" - 2 )"
     #printf "%s move %s %s 2\n" "_marker" "$(expr "$translatex")" "$(expr "$translatey" - 2 )" >/dev/stderr
     grep "$group" board | sed "s/$group/_marker2/g"
     printf "%s renamegroup _marker _marker%s%s\n" "$user" "$(date +%s)" "$RANDOM"
    fi
  done
