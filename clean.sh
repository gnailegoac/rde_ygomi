#!/bin/bash

set -e

this_srcipt_dir=$(dirname `readlink -f $0`)

#echo "to remove dir " ${this_srcipt_dir} "build, build_ut"
if [ -f cppcheck_junit.xml ]; then
	cp cppcheck_junit.xml out/
	rm -f cppcheck_junit.xml
else
	echo "Do you forget to do cppcheck?"
fi
rm -rf ${this_srcipt_dir}/build
rm -rf ${this_srcipt_dir}/build_ut
rm -rf ${this_srcipt_dir}/out

echo "clean temporary files done."