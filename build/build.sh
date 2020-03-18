#!/bin/bash
#
# Copyright (c) [2020] Huawei Technologies Co.,Ltd.All rights reserved.
#
# OpenArkCompiler is licensed under the Mulan PSL v1.
# You can use this software according to the terms and conditions of the Mulan PSL v1.
# You may obtain a copy of Mulan PSL v1 at:
#
#     http://license.coscl.org.cn/MulanPSL
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
# FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v1 for more details.
#
set -e
source build/envsetup.sh
make clean
option=$@
logfile_name=$(date +"%Y-%m-%d-%H-%M-%S")
logfile_path=${MAPLE_ROOT}/build/logs
date_str=$(date "+%Y-%m-%d %H:%M:%S")
echo "${date_str} INFO special log start" | tee ${logfile_path}/${logfile_name}.log
if [ "x${option}" = "xDEBUG" ]; then
  make BUILD_TYPE=DEBUG | tee -a ${logfile_path}/${logfile_name}.log
else
  make | tee -a ${logfile_path}/${logfile_name}.log
fi
date_str=$(date "+%Y-%m-%d %H:%M:%S")
echo "${date_str} INFO special log end" | tee -a ${logfile_path}/${logfile_name}.log
