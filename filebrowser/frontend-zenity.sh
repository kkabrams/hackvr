#!/bin/bash
stdbuf -oL $1 < p | stdbuf -oL tr '\n' ' ' | sed -u 's/$/_/' | stdbuf -oL tr '_' '\n' | xargs -L1 zenity --list --column file > p
