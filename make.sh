#!/usr/bin/bash

python /usr/lib/python2.6/site-packages/freeze/freeze.py \
 ./start.py -m encodings.ascii encodings.utf_8 encodings.utf_16_be
make
for i in *.{o,c}
  do
    rm ${i}
  done
rm ./Makefile