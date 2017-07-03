# !/bin/bash
  rm -rf ./bin/result_ut/result_ut_*.txt
  for ((i=1; i<200; i++))
  do
    echo $i
    #valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all -v ./bin/utest 2>&1 | tee ./bin/result_ut/result_ut_$i.txt 
    valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all -v ./bin/utest > ./bin/result_ut/result_ut_$i.txt 
    #./bin/utest
  done
