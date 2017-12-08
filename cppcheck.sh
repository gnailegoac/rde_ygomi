#!/bin/bash

exclude_files=""
if [ -f exclude_cppcheck_files ]; then
while read LINE
do
exclude_files="-i${LINE} ${exclude_files}"
done < exclude_cppcheck_files
echo $exclude_files
fi
cppcheck --xml-version=2 $exclude_files source test 2> cppcheck_report.xml
cppcheck_junit cppcheck_report.xml cppcheck_junit.xml
rm -f cppcheck_report.xml