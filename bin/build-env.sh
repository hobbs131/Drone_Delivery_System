#!/bin/bash

docker pull csci3081f20/base

CONTAINER=csci3081f20-env-container
ROOTDIR=`git rev-parse --show-toplevel`
docker build $1 \
	--target env \
	--build-arg USER_ID=$(id -u) \
	--build-arg GROUP_ID=$(id -g) \
	-t csci3081f20/env "$ROOTDIR"
rm -rf .dependencies
mkdir -p .dependencies
while [ "$(docker ps -aq -f name=${CONTAINER})" ]
do
    docker rm ${CONTAINER}
done
docker run --name=${CONTAINER} -v "${ROOTDIR}:/home/user/repo" csci3081f20/env cp -r ../../../project/grades/Fall-2020/csci3081/dependencies/include .dependencies

