#!/bin/sh

appname="RoadDatabaseExplorer"

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

LD_LIBRARY_PATH=$dirname/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

$dirname/$appname "$@"
