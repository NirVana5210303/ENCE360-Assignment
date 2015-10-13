#!/bin/bash

cat test.txt | while read in; do ./test_download.sh "$in"; done
