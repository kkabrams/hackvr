#!/bin/sh
#objectname addshape color points_in_shape
echo offsetshape: $1 $2 $3
awk '{ for(i=4;i<=(4+($4*3));i+=3){$i+='"$1"';$(i+1)+='"$2"';$(i+2)+='"$3"';} print $0;}'

