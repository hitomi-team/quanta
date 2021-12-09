#!/bin/sh

export CC=clang
export CXX=clang++

export CFLAGS="-Wall -Wextra -fno-plt -march=x86-64 -mtune=generic -pipe"
export CXXFLAGS="${CFLAGS}"

export LDFLAGS="-fuse-ld=lld -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now"
