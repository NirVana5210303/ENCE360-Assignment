#!/bin/bash

echo "n_threads,mine1,mine2,mine3,other1,other2,other3"

for ((threads=5; threads <= 70; threads=threads+5))
do
  for ((tests=1; tests <= 3; tests++))
  do
    mytime[tests]="$( TIMEFORMAT='%lE';time ( ./downloader $1 $threads download ) 2>&1 1>/dev/null )"
    othertime[tests]="$( TIMEFORMAT='%lE';time ( ./bin/downloader $1 $threads download ) 2>&1 1>/dev/null )"
  done
  echo "$threads,${mytime[1]:2:5},${mytime[2]:2:5},${mytime[3]:2:5},${othertime[1]:2:5},${othertime[2]:2:5},${othertime[3]:2:5}"
done
