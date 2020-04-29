#!usr/bin/env bash
dmesg -c > /dev/null
echo "TIME_MEASUREMENT.txt"
./main < ./OS_PJ1_Test/TIME_MEASUREMENT.txt
dmesg -c | grep Project

echo "FIFO_1.txt"
./main < ./OS_PJ1_Test/FIFO_1.txt
dmesg -c | grep Project

echo "PSJF_2.txt"
./main < ./OS_PJ1_Test/PSJF_2.txt
dmesg -c | grep Project

echo "RR_3.txt"
./main < ./OS_PJ1_Test/RR_3.txt
dmesg -c | grep Project

echo "SJF_4.txt"
./main < ./OS_PJ1_Test/SJF_4.txt
dmesg -c | grep Project
