#!/bin/bash

PWD=`pwd`
TOPDIR=${PWD}/..
BINDIR=${TOPDIR}/build/demo
KEYOPT=-g

CONFFILE=${PWD}/config.txt
RESFILE=${PWD}/res.txt

if [ $# -ne 3 ]; then
    echo "./run.sh <value1> <value2> <expected value>"
    exit 1
fi

if [ -f ${RESFILE} ]; then
    echo -n "" > ${RESFILE}
fi

IS_EXIST_DEC=`ps auxww | grep ./dec | grep -v grep`
IS_EXIST_EVL=`ps auxww | grep ./eval | grep -v grep`

if [ -z "${IS_EXIST_DEC}" ]; then
    echo "Start decryptor server"
    (cd ${BINDIR}/dec  && ./dec -g -t ${CONFFILE} 1> /dev/null 2>>${RESFILE} &)
fi

if [ -z "${IS_EXIST_EVL}" ]; then
    echo "Start evaluator server"
    (cd ${BINDIR}/eval && ./eval 2>&1 > /dev/null &)
fi

(cd ${BINDIR}/enc  && ./enc -t 0 ${1} 2>&1 > /dev/null)
(cd ${BINDIR}/enc  && ./enc -t 1 ${2} 2>&1 > /dev/null)

while [ -z ${RESFILE} ]; do sleep 0.1; done

RESULT=`cat ${RESFILE}`

TESTRES="NG"
if [ ${RESULT} = $3 ]; then
    TESTRES="OK"
fi

echo "Comparision (x <= y): x=$1, y=$2, exp=$3, res=${RESULT} ... ${TESTRES}"

