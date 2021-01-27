#!/bin/bash

PORT=$1

if [ -z "$project_port" ]
then
    project_port=8081
fi

if [ -z "$1" ]
then
    PORT=$project_port
fi

if [ -z "$USE_REPO_DIR" ]
then
    ROOTDIR=`git rev-parse --show-toplevel`
else
    ROOTDIR=/home/user/repo
fi

echo $ROOTDIR

gdb --args ${ROOTDIR}-build/bin/web-app $PORT web
