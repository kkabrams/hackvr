#!/bin/sh
#format of lines read from stdin:
#    $1        $2      $3         $4  <-- variables in awk
#objectname addshape color points_in_shape
awk '{ for(i=5;i<=(4+($4*3));i+=3){$i+='"$1"';$(i+1)+='"$2"';$(i+2)+='"$3"';} print $0;}'
