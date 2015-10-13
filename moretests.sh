#!/bin/bash

for((i=1; i < 10; i++))
do
  ./bigtests.sh test.txt > test_$i.csv
  ./bigtests.sh selection.txt > selection_$i.csv
done
