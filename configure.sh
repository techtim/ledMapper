#!/bin/bash

PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

for addon in $PROJECT_DIR/addons/*;
do 
	ln -s $addon $PROJECT_DIR/../../../addons/
done;

rm -rf addons/ofxDatGui/src/libs/ofxSmartFont/example-ofxSmartFont/