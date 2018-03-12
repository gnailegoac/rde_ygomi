#!/bin/bash

set -e

scriptDir=$(dirname `readlink -f $0`)
platform="Linux"
deployPath="deploy"
releasePath="release"

appName=$(grep -Po '(?<="name": ")[^"]*' config.json)
version=$(grep -Po '(?<="version": ")[^"]*' config.json)
groupId=$(grep -Po '(?<="groupId": ")[^"]*' config.json)
repositoryId=$(grep -Po '(?<="repositoryId": ")[^"]*' config.json)
url=$(grep -Po '(?<="url": ")[^"]*' config.json)

artifactId=${appName}"-"${platform}
appZipName=${appName}"-"${platform}".tar.gz"
fileName=${scriptDir}/${deployPath}/${appZipName}

echo "Publish $appZipName to maven..."
echo "artifactId: $artifactId"
echo "version: $version"
echo "groupId: $groupId"
echo "repositoryId: $repositoryId"
echo "url: $url"
echo "fileName: $fileName"

# publish to maven
mvn deploy:deploy-file -DgroupId=${groupId} \
    -DartifactId=${artifactId} \
    -Dversion=${version} \
    -Dpackaging=tar.gz \
    -Dfile=${fileName} \
    -DgeneratePom=true \
    -DrepositoryId=${repositoryId} \
    -Durl=${url} \
    -Dmaven.wagon.http.ssl.insecure=true \
    -Dmaven.wagon.http.ssl.allowall=true
