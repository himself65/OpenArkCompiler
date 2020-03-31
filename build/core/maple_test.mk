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
include $(MAPLE_BUILD_CORE)/maple_variables.mk

test: $(APP_S)
include $(MAPLE_BUILD_CORE)/mplcomb.mk
include $(MAPLE_BUILD_CORE)/genmplt.mk
include $(MAPLE_BUILD_CORE)/java2jar.mk

.PHONY: clean
clean:
	@rm -rf *.jar
	@rm -f *.class
	@rm -f *.mpl
	@rm -f *.mplt
	@rm -f *.s
	@rm -f *.groots.txt
	@rm -f *.primordials.txt
	@rm -rf comb.log
	@rm -rf *.muid
