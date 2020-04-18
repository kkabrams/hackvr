#!/usr/bin/env bash

set -o pipefail

OUR_HOST=$(/usr/local/libexec/sockip | head -n1)
THEIR_HOST=$(/usr/local/libexec/peerip | head -n1)
OUR_PORT=$(/usr/local/libexec/sockip | tail -n1)
THEIR_PORT=$(/usr/local/libexec/peerip | tail -n1)

log=/var/log/log.hackvr

# some hackvr scripts use USER for commanding the player
if USER=$(ident $OUR_HOST $THEIR_HOST 113 $THEIR_PORT $OUR_PORT);then
	export USER
else
	printf "%s does not have an ident daemon running.\n" "$THEIR_HOST" >> $log
	exit 1
fi

if [ ! "$USER" ];then
  exit 1
fi
printf "%s@%s connected to hackvr\n" "$USER" "$THEIR_HOST" >> $log

last_pong=$(date +%s)
wat=true

while true;do
	while read -t 10 group action target args;do
		printf "hackvr line: %s\n" "$group $action $target $args" >> $log
		if [ "$group" = "$USER" -a "$action" = "pong" ];then
			printf '%s@%s ponged on hackvr\n' $USER $THEIR_HOST >> $log
			last_pong="$(date +%s)"
		fi
		if [ "$group" = "$USER" -a "$action" = "quit" ];then
			break
		fi
		if [ "$action" = "action" ];then
			if [ "$target" = "/links" ];then
				printf "%s deleteallexcept %s\n" "$USER" "$USER"
				off=-10
				for link in gopher://thebackupbox.net/;do
					off=$[$off + 10]
					printf '<%s> addshape 4 3  0 0 0  0 8 0  8 0 0\n' "$link"
					printf '%s' "$link" | makelabel.sh "<$link>" 8 0 0
					printf '<%s> move 0 %d 0\n' "$link" "$off"
				done
				target=/
			fi
			if [ $target = "/radio" ];then
				printf "$USER deleteallexcept $USER\n"
				cd /var/hackvr/
				./radio.sh
				target=/
			fi
			if [ $target = "/ttt" ];then
				printf "$USER deleteallexcept $USER\n"
				cd /usr/local/src/hackvr/share/hackvr/examples/tictactoe
				./game.sh 2>/dev/null
				target=/
			fi
			if grep "^/dun" <<< "$target" 2>&1 >/dev/null;then
				printf "$USER deleteallexcept $USER\n"
				export seed="$(printf "%s\n" "$target" | cut -d/ -f3)"
				cd /usr/local/src/hackvr/share/hackvr/examples/dungen
				./dunexplore.sh 2>/dev/null
				target=/
			fi
			if [ $target = "/multi" ];then
				/var/hackvr/multi.sh
				target=/
			fi
			if [ $target = "/map" ];then
				printf "$USER deleteallexcept $USER\n"
				cd /usr/local/src/hackvr/share/hackvr/examples/anonet_map
				./map 2>/dev/null
				printf '/ addshape 4 3   0 0 0   0 8 0   8 0 0\n'
				while read group action target args;do
					if [ "$action" = "action" ];then
						if [ "$target" = "/" ];then
							break
						fi
					fi
				done
				target=/
			fi
			if [ $target = "/" ];then
				printf "%s deleteallexcept %s\n" "$USER" "$USER"
				cd /usr/local/src/hackvr/share/hackvr/examples/hackvrd
				off=-10
				for name in ttt dun map multi links radio;do
					off=$[$off + 10]
					printf '/%s addshape 4 3   0 0 0   0 8 0   8 0 0\n' "$name"
					printf '/%s' "$name" | makelabel.sh "/$name" 8 0 0
					printf '/%s move 0 %d 0\n' $name $off
				done
			fi
		else
			if $wat;then
				wat=false
				printf '/ addshape 4 3  0 0 0  0 8 0  8 0 0\n'
				printf 'welcome' | makelabel.sh / 8 0 0
			fi
		fi
	done
	#printf "pinging... %s@%s\n" "$USER" "$THEIR_HOST" >> /var/log/hackvr.all
	#printf "%s ping %s\n" "$USER" "thebackupbox.net"
	#if [ $[$(date +%s) - $last_pong] -gt 30 ];then #they probably pinged out.
	#	printf '# you pinged out. gotta respond to those, dude.\n'
	#	printf "%s@%s pinged out from hackvr\n" "$USER" "$THEIR_HOST" >> /var/log/hackvr.all
	#	exit 1
	#fi
done

printf "%s@%s disconnected from hackvr\n" "$USER" "$THEIR_HOST" >> $log
