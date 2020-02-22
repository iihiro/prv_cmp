#!/bin/bash

TOPDIR=`pwd`
BINDIR=${TOPDIR}/build/demo
KEYOPT=-g

while getopts s OPT; do
    case $OPT in
	"s" ) KEYOPT="" ;;
    esac
done

xterm -T "Dec"  -e "/bin/bash -c 'cd ${BINDIR}/dec    && ./dec ${KEYOPT}; exec /bin/bash -i'"&
xterm -T "EncA" -e "/bin/bash -c 'cd ${BINDIR}/enc    && ./enc 0 10;      exec /bin/bash -i'"&
xterm -T "EncB" -e "/bin/bash -c 'cd ${BINDIR}/enc    && ./enc 1  6;      exec /bin/bash -i'"&
xterm -T "Eval" -e "/bin/bash -c 'cd ${BINDIR}/eval   && ./eval;          exec /bin/bash -i'"&
