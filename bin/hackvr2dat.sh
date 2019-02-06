### just split into points for each line and and add two blank lines after each hackvr line
tr -s ' ' | cut '-d ' -f5- | sed 's/$/__/g' | tr ' ' '\n' | paste - - - | tr '_' '\n'
