#!/bin/sh
ss -tlanp src 127.0.0.1:1337 | tr ' ' '\n' | grep pid | tr , '\n' | grep pid | cut -d= -f2- | xargs kill
