#!/bin/bash

while read row; do
    v1=`echo ${row} | cut -d , -f 1`
    v2=`echo ${row} | cut -d , -f 2`
    ex=`echo ${row} | cut -d , -f 3`

    ./run.sh ${v1} ${v2} ${ex}
done < testcase.csv

