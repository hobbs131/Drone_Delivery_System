#!/bin/bash

PORT=$3
USER=$1
CPU=$2
LAB=$4

if [ -z "$4" ]
then
    LAB=kh1250
    #LAB=kh4250
    #LAB=kh1200
    #LAB=kh4240
    #LAB=kh4250
fi

ssh -L $PORT:127.0.0.1:$PORT $USER@csel-$LAB-$CPU.cselabs.umn.edu
