#!/usr/bin/env bash
log=/var/log/log.hackvr

button="-2 2 1  2 2 1  2 -2 1  -2 -2 1"

printf "<http://thebackupbox.net:8901/> addshape 6 4  -2 6 0  2 6 0  2 2 0  -2 2 0\n"
printf "<http* move up\n"
# |>
name=radio_play
printf "%s addshape 4 4  %s\n" "$name" "$button"
printf "radio_play addshape 4 3  -1 1 0  1 0 0 -1 -1 0\n"
printf "radio_play move -10 0 0\n"
# ||
name=radio_pause
printf "%s addshape 4 4  %s\n" "$name" "$button"
printf "radio_pause addshape 4 4  -1 1 0  -.5 1 0  -.5 -1 0  -1 -1 0\n"
printf "radio_pause addshape 4 4  .5 1 0  1 1 0  1 -1 0  .5 -1 0\n"
printf "radio_pause move -6 0 0\n"

# []
name=radio_stop
printf "%s addshape 4 4  %s\n" "$name" "$button"
printf "radio_stop addshape 4 4  -1 1 0  1 1 0  1 -1 0  -1 -1 0\n"
printf "radio_stop move -2 0 0\n"

# |>|
name=radio_next
printf "%s addshape 4 4  %s\n" "$name" "$button"
printf "radio_next addshape 4 3  -1 1 0   .75 0 0   -1 -1 0\n"
printf "radio_next addshape 4 4  .75 1 0  1 1 0  1 -1 0  .75 -1 0\n"
printf "radio_next move 2 0 0\n"

# |<|
name=radio_prev
printf "%s addshape 4 4  %s\n" "$name" "$button"
printf "radio_prev addshape 4 3  -.75 0 0  1 1 0  1 -1 0\n"
printf "radio_prev addshape 4 4  -1 1 0  -.75 1 0  -.75 -1 0  -1 -1 0\n"
printf "radio_prev move 6 0 0\n"

printf "exit addshape 4 4  -2 -5 0  2 -5 0  2 -9 0  -2 -9 0\n"

# a "listen" link to the http stream
# so anyone using hackvr_uri can open it from inside hackvr. :)

	while read group action target args;do
		printf "hackvr line: %s\n" "$group $action $target" >> $log
		if [ "$group" = "$USER" -a "$action" = "quit" ];then
			break
		fi
		if [ "$action" = "action" ];then
			if [ $target = "radio_pause" ];then
				mpc --host=21.41.41.4 --port=61666 pause 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "radio_play" ];then
				mpc --host=21.41.41.4 --port=61666 play 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "radio_prev" ];then
				mpc --host=21.41.41.4 --port=61666 prev 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "radio_next" ];then
				mpc --host=21.41.41.4 --port=61666 next 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "radio_stop" ];then
				mpc --host=21.41.41.4 --port=61666 stop 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "exit" ];then
				break;
			fi
		fi
	done
