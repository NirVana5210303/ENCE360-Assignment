#!/bin/bash

echo "n_threads,time (my program), time (supplied program)"

for ((threads=5; threads <= 70; threads=threads+5))
do
  mytime="$( TIMEFORMAT='%lE';time ( ./downloader $1 $threads download ) 2>&1 1>/dev/null )"
  othertime="$( TIMEFORMAT='%lE';time ( ./bin/downloader $1 $threads download ) 2>&1 1>/dev/null )"
  echo "$threads,${mytime:2:5},${othertime:2:5}"
done
