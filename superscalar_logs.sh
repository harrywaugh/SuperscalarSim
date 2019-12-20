!/bin/bash

make superscalar-stats-2bit-btb > /dev/null
for program in programs/*.asm programs/*.benchmark
do
  echo "Getting superscalar (btb) logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  mkdir -p results/$filename
  ./simulator.exe $program > results/$filename/superscalar-8way2.log
done
