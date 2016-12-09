#!/bin/sh
traceroute -n $1 | tail -n+2 | cut '-d ' -f4 | ./IPs_to_triangle.sh
