#!/usr/bin/env bash
printf "%s move 0 0 -10\n" "$USER"
printf "square addshape 2 4 1 1 0  -1 1 0  -1 -1 0  1 -1 0\n"
inc=5
end=180
while true;do
  for i in $(seq 1 $inc $end);do
    printf "square rotate +%d +0 +0\n" "$inc"
    sleep .01
  done
  for i in $(seq 1 $inc $end);do
    printf "square rotate +0 +%d +0\n" "$inc"
    sleep .01
  done
  for i in $(seq 1 $inc $end);do
    printf "square rotate +0 +0 +%d\n" "$inc"
    sleep .01
  done
done
