!/bin/bash

make stats > /dev/null
for program in programs/*.asm programs/*.benchmark;
do
  echo "Getting non pipelined logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  mkdir -p results/$filename
  ./simulator.exe $program > results/$filename/non-pipelined.log
done


make pipelined-stats > /dev/null
for program in programs/*.asm programs/*.benchmark
do
  echo "Getting pipelined logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  ./simulator.exe $program > results/$filename/pipelined.log
done

make superscalar-stats-2bit > /dev/null
for program in programs/*.asm programs/*.benchmark
do
  echo "Getting superscalar (2bit) logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  ./simulator.exe $program > results/$filename/superscalar.log
done

make superscalar-stats-NT > /dev/null
for program in programs/*.asm programs/*.benchmark
do
  echo "Getting superscalar (!taken) logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  ./simulator.exe $program > results/$filename/superscalar-nt.log
done

make superscalar-stats-T > /dev/null
for program in programs/*.asm programs/*.benchmark
do
  echo "Getting superscalar (taken) logs for $program program.."
  filename="${program%.*}"
  filename=${filename##*/}
  ./simulator.exe $program > results/$filename/superscalar-t.log
done
