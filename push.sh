#!/bin/bash

cd ./test
make clean
cd -

git add *
git commit -m "modify files"
git push


