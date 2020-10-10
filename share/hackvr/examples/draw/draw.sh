#!/usr/bin/env bash
echo "$USER move 5 7 -19"

for y in $(seq 0 1 15);do
 for x in $(seq 0 1 15);do
  echo cell_${x}_${y} addshape 2 4 -.5 -.5 0 -.5 .5 0 .5 .5 0 .5 -.5 0
  #echo cell_${x}_${y} addshape 2 4 $x $y 0 $[x] $[y+1] 0 $[x+1] $[y+1] 0 $[x+1] $[y] 0
  echo cell_${x}_${y} move $x $y 0
 done
done

### color pallete
### 0-8 are ansi colors.
for c in $(seq 0 16);do
	echo color_${c} addshape $[c] 4   $[-2 - ($c / 8)] $[($c%8)+3] 0   $[-1 - ($c / 8)] $[($c%8)+3] 0   $[-1 - ($c / 8)] $[($c%8)+4] 0   $[-2 - ($c / 8)] $[($c%8)+4] 0
done

#cat ${PREFIX}/share/hackvr/meshes/floppy.hackvr | grep '^[^ #]' | sed 's/^[^ ][^ ]*/save/g'
#echo save move -5 0 0

tee /dev/stderr | while read -r source command target;do
  if [ "$command" = "action" ];then
    if echo "$target" | grep "^save" 2>&1 >/dev/null;then
      echo "$USER export *"
      date=$(date +%s)
      echo "$USER ping $date"
      while read -r line;do
        if [ "$line" = "${USER} pong ${date}" ];then
          break
        else
          printf "%s\n" "$line" >> ~/drawsave.hackvr
	fi
      done
    fi
    if echo "$target" | grep "^color_" 2>&1 >/dev/null;then
      color=$(echo $target | cut -d_ -f2)
      echo deletegroup $target
      echo color_${color}_current addshape $[$color] 4   -2 0 0 -2 1 0 -1 1 0 -1 0 0
    elif echo "$target" | grep '^cell_' 2>&1 >/dev/null;then
      x=$(echo $target | cut -d_ -f2)
      y=$(echo $target | cut -d_ -f3)
      echo $target deletegroup $target
      #echo $target addshape $[$color] 4 $x $y 0 $x $[y+1] 0 $[x+1] $[y+1] 0 $[x+1] $y 0 | tee /dev/stderr
      echo $target addshape $[$color] 4 -.5 -.5 0 -.5 .5 0 .5 .5 0 .5 -.5 0
      printf "%s move %d %d 0" "$target" "$x" "$y"
    fi
  fi
done
