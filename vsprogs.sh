#!/bin/bash



./bin/downloader $1 12 download > __template
./downloader $1 12 download > __output

diff <(sort __template) <(sort __output) --report-identical-files --brief

rm __template
rm __output
