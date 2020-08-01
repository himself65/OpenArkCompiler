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

JAVA_CORE=${MAPLE_ROOT}/libjava-core/java-core.jar
cd ${MAPLE_ROOT}/libjava-core
${MAPLE_ROOT}/output/bin/mplfe ${JAVA_CORE} -t -o java-core
cd -
rm -rf ${MAPLE_ROOT}/report
${MAPLE_ROOT}/output/bin/mplfeUT ext -gen-base64 ${MAPLE_ROOT}/src/mplfe/test/jbc_input/JBC0001/Test.class
${MAPLE_ROOT}/output/bin/mplfeUT ext -in-class ${MAPLE_ROOT}/src/mplfe/test/jbc_input/JBC0001/Test.class
${MAPLE_ROOT}/output/bin/mplfeUT ext -in-jar ${JAVA_CORE}
${MAPLE_ROOT}/output/bin/mplfeUT ext -mplt ${MAPLE_ROOT}/libjava-core/java-core.mplt
${MAPLE_ROOT}/output/bin/mplfeUT testWithMplt ${MAPLE_ROOT}/libjava-core/java-core.mplt

