#!/bin/bash


wget -O wget.pdf $1 -q
./http_download $1 http.pdf

diff wget.pdf http.pdf --report-identical-files --brief
