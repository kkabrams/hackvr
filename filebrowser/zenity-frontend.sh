#!/bin/sh
cat p | ./filebrow.sh | tr -d '\n' | xargs -L1  zenity --list --column file > p
