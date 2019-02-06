#!/bin/sh
exec $* | xclip -i | xclip -o | xmessage -buttons ok:0,pipe:1 -file - || exit 0
#ask for command
xclip -o | eval $(zenity --entry)
