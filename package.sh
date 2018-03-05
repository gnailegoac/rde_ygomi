#!/bin/bash

set -e

mode="snapshot"
for i in "$@"
do
    case $i in
        -f)
            mode="full"
            ;;
        -s)
            mode="snapshot"
            ;;
        -r)
            mode="release"
            ;;
        -h)
            echo "Usage:"
            echo -e "$0 [-srf]"
            echo -e "\t-s\tsnapshot mode: package raw output."
            echo -e "\t-r\trelease mode: package all dependency."
            echo -e "\t-r\tfull mode: both release and snapshot."
            echo -e "default mode: snapshot"
            exit 0
            ;;
    esac
done

scriptDir=$(dirname `readlink -f $0`)
platform="Linux"
deployPath="deploy"
releasePath="release"

appName=$(grep -Po '(?<="name": ")[^"]*' config.json)
version=$(grep -Po '(?<="version": ")[^"]*' config.json)
dependencyUrl=$(grep -Po '(?<="dependencyUrl": ")[^"]*' config.json)

# install app to $scriptDir/deploy/release
rm -rf ${scriptDir}/${deployPath}
echo "Install $appName..."
cd ${scriptDir}/build
make install
# copy resource file to deploy folder
cp -a resource ${scriptDir}/${deployPath}/${releasePath}
cd ${scriptDir}/${deployPath}

echo "create package in $mode Mode."

if [ "${mode}" == "snapshot"  ] || [ "${mode}" == "full" ]
then
    snapShotAppZipName=${appName}"-"${platform}"-snapshot-v"${version}".tar.gz"
    echo "create snapshot package $snapShotAppZipName in $scriptDir/$deployPath"
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
    tar -zcvf $snapShotAppZipName $releasePath
    echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
fi

if [ "${mode}" == "release"  ] || [ "${mode}" == "full" ]
then
    wget --no-check-certificate ${dependencyUrl} -O RoadDBExplorerDependency.tar.gz
    tar zxf RoadDBExplorerDependency.tar.gz

    echo "Copy dependency file from RoadDBExplorerDependency to $releasePath."
    cp -raf ./RoadDBExplorerDependency/* ./${releasePath}
    echo "Copy packaging files to $releasePath ."
    cp ${scriptDir}/package/* ./${releasePath}

    appZipName=${appName}"-"${platform}".tar.gz"
    appFolderName=${appName}"-v"${version}
    mv $releasePath $appFolderName
    echo "create package $appZipName in $scriptDir/$appFolderName"
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
    tar -zcvf $appZipName $appFolderName
    echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
fi
