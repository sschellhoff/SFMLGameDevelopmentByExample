#!/bin/bash

echo "building chapter_01"
cd chapter_01/1.\ Opening\ the\ window/
./build.sh
cd ../2.\ Rendering\ the\ rectangle/
./build.sh
cd ../3.\ Drawing\ a\ sprite/
./build.sh
cd ../..

echo "building chapter_02"
cd chapter_02/1.\ Core\ game\ structure
./build.sh
cd ../..

echo "building chapter_03"
cd chapter_03/
./build.sh
cd ..

echo "building chapter_04"
cd chapter_04/
./build.sh
cd ..

echo "building chapter_05"
cd chapter_05/
./build.sh
cd ..

echo "building chapter_06"
cd chapter_06/
./build.sh
cd ..

echo "building chapter_07"
cd chapter_07/
./build.sh
cd ..

echo "building chapter_08"
cd chapter_08/
./build.sh
cd ..

echo "building chapter_09"
cd chapter_09/
./build.sh
cd ..

echo "building chapter_10"
cd chapter_10/
./build.sh
cd ..

echo "done"