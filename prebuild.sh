#!/bin/bash

set -e

outputHeader="src/AppMeta.h"

version=$( sed -n 's/.*"version": "\(.*\)",/\1/p' config.json)
gitCommitCount=`git rev-list --count HEAD`
gitBranchName=`git rev-parse --abbrev-ref HEAD`
gitCommitSha=`git rev-parse HEAD`

echo "Version: $version"
echo "Commit count: $gitCommitCount"
echo "Branch name: $gitBranchName"
echo "Revision: $gitCommitSha"
echo "Default Server: $gmmDefaultServer"

echo "// This file is generated automatically by prebuild.sh, please don't modify it mannully." > $outputHeader
echo "#pragma once" >> $outputHeader
echo "#include <string>" >> $outputHeader
echo "namespace AppMeta" >> $outputHeader
echo "{" >> $outputHeader
echo "    const std::string scVersion=\"${version}\";" >> $outputHeader
echo "    const int scGitCommmitCount=${gitCommitCount};" >> $outputHeader
echo "    const std::string scGitBranchName=\"${gitBranchName}\";" >> $outputHeader
echo "    const std::string scGitCommitSha=\"${gitCommitSha}\";" >> $outputHeader
echo "    const std::string scDefaultServer=\"${gmmDefaultServer}\";" >> $outputHeader
echo "}" >> $outputHeader
