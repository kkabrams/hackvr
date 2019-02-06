#!/bin/bash
echo "$USER move 0 0 -100"

for y in $(seq 1 1 48);do
 for x in $(seq 1 1 48);do
  echo ${x}_${y} addshape 2 4 $x $y 0 $[x] $[y+1] 0 $[x+1] $[y+1] 0 $[x+1] $[y] 0
 done
done

### color pallete
### 0-8 are ansi colors. +16 to make them not affected by "lighting"
for c in $(seq 0 8);do
  echo color_${c} addshape $[c+16] 4 -2 ${c+2} 0 -1 ${c+2} 0 -1 $[c+3] 0 -2 $[c+3] 0
done

echo "derp" >&2

color=3
echo color_${color} addshape $[color+16] 4 -2 0 0 -2 1 0 -1 1 0 -1 0 0

while read source command target;do
  printf "target: %s command: %s\n" "$target" "$command" >&2
  if [ "$command" = "action" ];then
    if echo "$target" | grep "^color_" 2>&1 >/dev/null;then
      color=$(echo $target | cut -d_ -f2)
      echo deletegroup $target
      echo color_${color}_current addshape $[color+16] 4 -2 -2 0 -2 -1 0 -1 -1 0 -1 -2 0
    else 
      echo "derp" >&2
      x=$(echo $target | cut -d_ -f1)
      y=$(echo $target | cut -d_ -f2)
      echo $target deletegroup $target
      echo $target addshape $[color + 16] 4 $x $y 0 $x $[y+1] 0 $[x+1] $[y+1] 0 $[x+1] $y 0 | tee /dev/stderr
    fi
  fi
done
