#!/bin/bash

if [ -z "$USE_REPO_DIR" ]
then
    ROOTDIR=`git rev-parse --show-toplevel`
else
    ROOTDIR=/home/user/repo
fi

echo $ROOTDIR

${ROOTDIR}-build/test/csci3081tests
