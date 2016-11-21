#!/bin/sh
grep --line-buffered ^epoch | stdbuf -oL cut '-d ' -f2- | grep --line-buffered ^action | stdbuf -oL cut '-d ' -f2-
