#!/usr/bin/env bash
hour=$(date +%H | sed 's/^0//')
minute=$(date +%M | sed 's/^0//')
second=$(date +%S | sed 's/^0//')
printf "_clock_face_hand_hour_ rotate 0 0 -%s\n" "$[hour * 30]"
printf "_clock_face_hand_minute_ rotate 0 0 -%s\n" "$[minute * 6]"
printf "_clock_face_hand_second_ rotate 0 0 -%s\n" "$[second * 6]"
