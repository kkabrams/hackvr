#!/bin/sh
exec $* | xmessage -file - &
