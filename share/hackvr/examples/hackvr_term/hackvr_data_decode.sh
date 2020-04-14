#!/usr/bin/env bash
set -eo pipefail
exec grep --line-buffered '^[^ ]* data ' | stdbuf -oL tr -s ' ' | stdbuf -oL cut '-d ' -f3- | stdbuf -o0 xxd -r -p
