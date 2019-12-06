!/bin/bash

make non-pipelined-stats > /dev/null
for program in programs/*.asm programs/*.benchmark;
do
  echo "Getting non pipelined logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  mkdir -p results/$filename
  ./simulator.exe $program > results/$filename/non-pipelined.log
done


make stats > /dev/null
for program in programs/*.asm programs/*.benchmark
do
  echo "Getting non pipelined logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  ./simulator.exe $program > results/$filename/pipelined.log
done
