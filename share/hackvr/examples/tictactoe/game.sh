#!/usr/bin/env bash
turn=$( expr $RANDOM % 2)
cat camera.pos board
echo "go. player: $turn" >&2
grep --line-buffered action \
  | while read user derp group;do
    echo FUCK >&2
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
     translatex="$(grep "$group" board | grep -v '^#' | tr -s ' ' | cut '-d ' -f5)"
     translatey="$(grep "$group" board | grep -v '^#' | tr -s ' ' | cut '-d ' -f6)"
     cat "marker$turn"
     printf "%s move %s %s 2\n" "_marker" "$(expr "$translatex")" "$(expr "$translatey" - 2 )"
     #printf "%s move %s %s 2\n" "_marker" "$(expr "$translatex")" "$(expr "$translatey" - 2 )" >/dev/stderr
     grep "$group" board | sed "s/$group/_marker2/g" #this is to make it still show the square around it.
     printf "%s renamegroup _marker _marker%s%s\n" "$user" "$(date +%s)" "$RANDOM"
    fi
  done