#!/bin/bash

set -e

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(dirname "$0")/build_ut

if [ -d "unit_test" ]; then
    ./build_ut/unit_test --gtest_output=xml
fi
