#!/bin/sh
#this is so I can offer a TCP server that'll
echo 'starting headless hackvr sending to multicast group 234.5.6.7 udp port 1337...'
ncat -u 234.5.6.7 1337 -c '../src/hackvr_headless world < in_pipe' &
echo 'starting TCP listener for clients to connect to on port 1337...'
ncat -v -k -l -p 1337 -c 'stdbuf -i0 cat > in_pipe | mcast 234.5.6.7 1337'
wait
echo 'all done.'
