#!/usr/bin/env bash
log=/var/log/log.hackvr

button="-2 2 1  2 2 1  2 -2 1  -2 -2 1"
bc=2
printf "<http://thebackupbox.net:8901/> addshape 6 4  %s\n" "$button"
printf "<http://thebackupbox.net:8901/> move -5 5 0\n"
# |>
name=play
printf "%s addshape %s 4  %s\n" "$name" "$bc" "$button"
printf "play addshape 4 3  -1 1 0  1 0 0 -1 -1 0\n"
printf "play move 0 0 0\n"
# ||
name=pause
printf "%s addshape %s 4  %s\n" "$name" "$bc" "$button"
printf "pause addshape 4 4  -1 1 0  -.5 1 0  -.5 -1 0  -1 -1 0\n"
printf "pause addshape 4 4  .5 1 0  1 1 0  1 -1 0  .5 -1 0\n"
printf "pause move 0 4 0\n"

# []
name=stop
printf "%s addshape %s 4  %s\n" "$name" "$bc" "$button"
printf "stop addshape 4 4  -1 1 0  1 1 0  1 -1 0  -1 -1 0\n"
printf "stop move 0 -4 0\n"

# |>|
name=next
printf "%s addshape %s 4  %s\n" "$name" "$bc" "$button"
printf "next addshape 4 3  -1 1 0   .75 0 0   -1 -1 0\n"
printf "next addshape 4 4  .75 1 0  1 1 0  1 -1 0  .75 -1 0\n"
printf "next move 4 0 0\n"

# |<|
name=prev
printf "%s addshape %s 4  %s\n" "$name" "$bc" "$button"
printf "prev addshape 4 3  -.75 0 0  1 1 0  1 -1 0\n"
printf "prev addshape 4 4  -1 1 0  -.75 1 0  -.75 -1 0  -1 -1 0\n"
printf "prev move -4 0 0\n"

printf "exit addshape 4 4  %s\n" "$button"
printf "exit move 5 5 0\n"

spin() {
	for r in $(seq 15 15 360);do
		printf "%s rotate 0 0 %s\n" "$1" "$r"
		sleep .1
	done
}

# a "listen" link to the http stream
# so anyone using hackvr_uri can open it from inside hackvr. :)

	while read group action target args;do
		printf "hackvr line: %s\n" "$group $action $target" >> $log
		if [ "$group" = "$USER" -a "$action" = "quit" ];then
			break
		fi
		if [ "$action" = "action" ];then
			if [ $target = "pause" ];then
				spin $target &
				mpc --host=21.41.41.4 --port=61666 pause 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "play" ];then
				spin $target &
				mpc --host=21.41.41.4 --port=61666 play 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "prev" ];then
				spin $target &
				mpc --host=21.41.41.4 --port=61666 prev 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "next" ];then
				spin $target &
				mpc --host=21.41.41.4 --port=61666 next 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "stop" ];then
				spin $target &
				mpc --host=21.41.41.4 --port=61666 stop 2>&1 >/dev/null 2>&1
			fi
			if [ $target = "exit" ];then
				spin $target
				break;
			fi
		fi
	done
