#!/usr/bin/env bash
## you might want to edit these to suit your tastes.
width=10
height=$width
words_per_puzzle=$[width+2]
x=$[-10 * $width / 2]
y=$[10 * $width / 2]
z=0
clicked=""
wordlist=/usr/share/dict/words
echo derp
wordsearch -w <(shuf $wordlist | grep '^.\{1,'"$width"'\}$' | head -n $words_per_puzzle | tee /dev/stderr) --columns $width --rows $height --text | tail -n+4 | head -n $height | while read -r line;do
  echo "$line" | tr -d ' ' | fold -w 1 | while read letter;do
    name="_${x}_${y}"
    printf "%s addshape 2 4 %s %s %s  %s %s %s  %s %s %s  %s %s %s\n" \
            "$name" \
            $[$x-2] $[$y-3] $z \
            $[$x+8] $[$y-3] $z \
            $[$x+8] $[$y+7] $z \
            $[$x-2] $[$y+7] $z
    echo $letter | makelabel.sh "$name" $x $y $z
    x=$[$x+10]
  done
  y=$[$y-10]
done
while read group action args;do
  if [ $action = action ];then
    if grep "$args" <<< "$clicked" 2>&1 >/dev/null;then
      clicked=$(grep -v '^'"$args"'$' <<< "$clicked")
      printf "%s move 0 0 0\n" "$args"
    else
      clicked="$clicked"$'\n'"$args"
      printf "%s move 0 0 -5\n" "$args"
    fi
#    xmessage -nearmouse "$args"
  fi
done
