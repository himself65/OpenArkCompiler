/*
 * Copyright (c) [2020] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * OpenArkCompiler is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest13
 *- @TestCaseName: LiverangeSubsumeRCTest13
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:检测在跳转语句（try---多catch）控制流中,当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 * -#step1:str2 = str;空指针的catch包含不住，不做优化外；另外两个catch都应该做incRef和decRef的优化。
 *  校验中间文件LiverangeSubsumeRCTest13.VtableImpl.mpl中无IncRef。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest13.java
 *- @ExecuteClass: LiverangeSubsumeRCTest13
 *- @ExecuteArgs:
 */

public class LiverangeSubsumeRCTest13 {
    public static void main(String[] argv) {
        int result = 2;
        boolean check;
        String str = "123#";
        String str2 = str;
        try {
            str.notifyAll();
            str = str2;
            Integer.parseInt(str);
        } catch (NumberFormatException e) {
            str = "123#456";  // 2
            result--;
        } catch (NullPointerException e) {
            str2 = "123456";  // 3
            result = 2;
        } catch (IllegalMonitorStateException e) {
            str2 = str;
            result = 0;
        }
        if (result == 0) {
            System.out.println("ExpectResult");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n