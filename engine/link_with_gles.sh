#!/bin/bash
clang++ -shared -fPIC -O2 -std=c++17 -I$PREFIX/include -I. \
  -o out/arm64-v8a/libNOVA3_neon.so \
  stub/jni_stubs.cpp \
  engine/glf_app.o \
  engine/nova3_io.o \
  engine/cmemory_stream.o \
  -llog -Wl,--no-as-needed -lGLESv2 -Wl,--as-needed
