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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest14
 *- @TestCaseName: LiverangeSubsumeRCTest14
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 *  -#step1: str2 = str1;这俩指向同一个字符串地址。
 *  -#step2: 在try里有str1 = str2; 仍然是str1的生命周期包含住了str2；所以str2的incref和decRef都会被优化掉
 *  校验中间文件LiverangeSubsumeRCTest14.VtableImpl.mpl中无IncRef。
 *- @Expect:ExpectResultExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest14.java
 *- @ExecuteClass: LiverangeSubsumeRCTest14
 *- @ExecuteArgs:
 */

public class LiverangeSubsumeRCTest14 {
    public static void main(String[] args) {
        String str1 = "ExpectResult";
        String str2 = str1;
        try {
            String temp = str2.substring(0, 3);
            temp = str1.toLowerCase();
            str1 = str2;
            temp = temp.equals("ExpectResult") ? temp : str1;
            temp.notifyAll();
            str1.notifyAll();
        } catch (Exception e) {
            System.out.print(str1);
        } finally {
            System.out.println(str1);
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResultExpectResult\n