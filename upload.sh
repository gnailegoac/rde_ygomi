#!/bin/bash

set -e

script_dir=$(dirname `readlink -f $0`)
proj_name=$(grep -Po '(?<="name" : ")[^"]*' config.json)
major_version=$(grep -Po '(?<="major_version" : ")[^"]*' config.json)
out_path=$(grep -Po '(?<="outdir" : ")[^"]*' config.json)
dev_user=$(grep -Po '(?<="dev_user" : ")[^"]*' config.json)
channel=$(grep -Po '(?<="channel" : ")[^"]*' config.json)
conan_user=$1
conan_passwd=$2
conan_server=$(grep -Po '(?<="conan_server" : ")[^"]*' config.json)
commit_id=`git rev-parse --short HEAD`
echo ${commit_id}
dt=$(date +%Y%m%d%H%M%S)
cur_version=${major_version}"."${dt}"."${commit_id}
echo ${cur_version}
echo ${proj_name}
cd ${script_dir}/build/
make install
cd ${script_dir}

rm -rf $script_dir/upload
mkdir -p $script_dir/upload
cp upload.template $script_dir/upload/conanfile.py
#--please specify what you want to upload and copy from out folder to upload------#
#cp out/${proj_name} $script_dir/upload/
cp -r ${out_path}/* $script_dir/upload/
#----------------------------#
cd $script_dir/upload
sed -i "s/PROJNAME/${proj_name}/g;s/CURRENT_VERSION/${cur_version}/g" conanfile.py
conan export ${dev_user}/${channel}
rm conanfile.py
touch conanfile.txt
echo "[requires]">>conanfile.txt
echo "${proj_name}/${cur_version}@${dev_user}/${channel}">>conanfile.txt
echo >>conanfile.txt
echo "[generators]">>conanfile.txt
echo "cmake">>conanfile.txt
conan install . --build
conan user -p ${conan_passwd} -r ${conan_server} ${conan_user}
conan upload ${proj_name}/${cur_version}@${dev_user}/${channel} -r ${conan_server}
cd ..
rm -rf $script_dir/upload