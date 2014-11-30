#!/bin/bash

mkdir -p build
cd build
cmake -G "Xcode" ..
cd ..
open build/openrct2.xcodeproj