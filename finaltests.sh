#!/bin/bash

#for((i=1; i < 70; i=i+5)) # i = num threads
#do
  #echo -n "mine,$i,"
  #for ((j=1; j < 5; j++)) # j = trials
  #do
    #foo="$(/usr/bin/time -f %E ./downloader large.txt 20 foo 2>&1 1>/dev/null)"
    #echo -n "${foo:2:7},"
  #done
  #echo ""
#done


for((i=61; i < 70; i=i+5)) # i = num threads
do
  echo -n "other,$i,"
  for ((j=1; j < 5; j++)) # j = trials
  do
    foo="$(/usr/bin/time -f %E ./bin/downloader large.txt 20 foo 2>&1 1>/dev/null)"
    echo -n "${foo:2:7},"
  done
  echo ""
done
