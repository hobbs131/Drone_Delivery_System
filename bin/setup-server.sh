#!/bin/bash

#./bin/server.sh /home/dtorban/csci3081/server 8081 dtorban 03 kh1250

ROOTDIR=`git rev-parse --show-toplevel`

USER=$1
CPU=$2
LAB=$3

if [ -z "$3" ]
then
    LAB=kh1250
    #LAB=kh4250
    #LAB=kh1200
    #LAB=kh4240
    #LAB=kh4250
fi

REMOTE_PROJ_DIR=/home/$USER/temp/csci3081

ssh $USER@csel-$LAB-$CPU.cselabs.umn.edu "mkdir -p $REMOTE_PROJ_DIR"
#rsync -rv $ROOTDIR/project $USER@csel-$LAB-$CPU.cselabs.umn.edu:$REMOTE_PROJ_DIR
scp -r "$ROOTDIR/project" $USER@csel-$LAB-$CPU.cselabs.umn.edu:$REMOTE_PROJ_DIR

