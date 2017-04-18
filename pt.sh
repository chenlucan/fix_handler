# !/bin/bash
  rm -rf ./result_ut/result_ut_*.txt
  for ((i=1; i<200; i++))
  do
    echo $i
    valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all -v ./bin/utest > ./result_ut/result_ut_$i.txt 2>&1
    #./bin/utest
  done
