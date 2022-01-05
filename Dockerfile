# https://github.com/kirshanthans/cilk-docker/blob/master/Dockerfile - for a docker container with cilk

FROM ubuntu:16.04

# adding apt repos cmd
RUN apt-get update && \
    apt-get install -y software-properties-common

# install tapir toolchain from repo
RUN apt-add-repository ppa:wsmoses/tapir-toolchain && \
    apt-get update && \
    apt-get install -y tapirclang-5.0 libcilkrts5

# install gdb and valgrind for debugging
RUN apt install -y gdb valgrind

# install update alternatives
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-5.0 10 \
    --slave /usr/bin/clang++ clang++ /usr/bin/clang++-5.0 && \
    update-alternatives --config clang

RUN mkdir -p /test

WORKDIR /test