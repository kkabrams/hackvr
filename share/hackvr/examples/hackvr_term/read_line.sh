#!/usr/bin/env bash
./hackvr_data_decode.sh \
       | ./pty `which bash` "-c" 'read -p "type> " line;xmessage $line' \
       | ./hackvr_term 60 2 \
       | sed -u 's/addshape 15 4/addshape 12 4/g'
