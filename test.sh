#!usr/bin/env bash
dmesg -c > /dev/null
typelist="FIFO RR SJF PSJF"
for type in $typelist;
do
    for i in  $(seq 1 5);
    do
        ./main < ./OS_PJ1_Test/$type"_"$i.txt > ./output/$type"_"$i"_stdout.txt"
        dmesg -c | grep Project > ./output/$type"_"$i"_dmesg.txt"
    done
done
./main < ./OS_PJ1_Test/TIME_MEASUREMENT.txt > ./output/"TIME_MEASUREMENT_stdout.txt"
dmesg -c | grep Project > ./output/"TIME_MEASUREMENT_dmesg.txt"