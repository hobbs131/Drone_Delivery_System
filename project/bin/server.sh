#!/bin/bash

#./bin/server.sh /home/dtorban/csci3081/server 8081 dtorban 03 kh1250

ROOTDIR=`git rev-parse --show-toplevel`

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

REMOTE_PROJ_DIR=/home/$USER/temp/csci3081

ssh $USER@csel-$LAB-$CPU.cselabs.umn.edu "mkdir -p $REMOTE_PROJ_DIR"
#rsync -rv $ROOTDIR/project $USER@csel-$LAB-$CPU.cselabs.umn.edu:$REMOTE_PROJ_DIR
scp "$ROOTDIR/project/*" $USER@csel-$LAB-$CPU.cselabs.umn.edu:$REMOTE_PROJ_DIR/project
scp -r "$ROOTDIR/project/src" $USER@csel-$LAB-$CPU.cselabs.umn.edu:$REMOTE_PROJ_DIR/project
ssh -L $PORT:127.0.0.1:$PORT $USER@csel-$LAB-$CPU.cselabs.umn.edu "cd $REMOTE_PROJ_DIR/project; git init .; ls; make clean; make; ./bin/run.sh $PORT"

