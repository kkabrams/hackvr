#!/usr/bin/env bash
turn=$( expr $RANDOM % 2)
BOARDFILE=board_orig
cat camera.pos $BOARDFILE
echo "go. player: $turn" >&2
grep --line-buffered action \
  | while read user action group;do
    if [ $action = "action" ];then
      if grep "_reset" <<<$group >/dev/null;then
       printf "%s deleteallexcept %s\n" "$user" "$user"
       cat $BOARDFILE
       continue
      elif grep "_exit" <<<$group >/dev/null;then
       #if a program wants to exit, it should just exit.
       #the program may have been ran by some other program.
       #maybe just filter out server-side quit messages for your own username?
       #any user quit will cause any hackvr to quit?
       #printf "%s quit\n" "$user"
       exit 0
      elif grep "_" <<<$group >/dev/null;then
       echo CUT IT OUT > /dev/stderr
       continue
      elif grep "^board" <<<$group >/dev/null;then
       turn=$(expr \( $turn + 1 \) % 2)
       #printf "# turn: %d\n" "$turn" > /dev/stderr
       printf "%s deletegroup %s\n" "$user" "$group"
       #printf "$user deletegroup %s\n" "$group" > /dev/stderr
#need to get the first point of the group clicked and translate the new shape by that much
       translatex="$(grep "$group" $BOARDFILE | grep -v '^#' | tr -s ' ' | cut '-d ' -f5)"
       translatey="$(grep "$group" $BOARDFILE | grep -v '^#' | tr -s ' ' | cut '-d ' -f6)"
       cat "marker$turn" | offsetshape.sh $translatex "$(expr "$translatey" - 2 )" 0
       grep "$group" $BOARDFILE | sed "s/$group/_marked/g"
      else
       xmessage "what? $group"
      fi
    fi
  done
