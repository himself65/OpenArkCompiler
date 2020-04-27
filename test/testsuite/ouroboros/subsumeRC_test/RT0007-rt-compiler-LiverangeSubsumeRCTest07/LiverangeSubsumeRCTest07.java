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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest07
 *- @TestCaseName: LiverangeSubsumeRCTest07
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作:自成环的场景应该会被优化
 *  -#step1: test07与test07./，而且test07.next的生命周期被test07包含，所以 test07.next的incref和dec ref都应该被优化掉。
 *  -#step2: 这样RC的自成环问题会被优化掉，该种场景仅通过RC就可以做到内存回收
 *  校验中间文件LiverangeSubsumeRCTest07.VtableImpl.mpl中无IncRef。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest07.java
 *- @ExecuteClass: LiverangeSubsumeRCTest07
 *- @ExecuteArgs:
 */
public class LiverangeSubsumeRCTest07 {
    LiverangeSubsumeRCTest07 next;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest07(String strObjectName) {
        next = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    public static void main(String[] args) {
        LiverangeSubsumeRCTest07 test07 = new LiverangeSubsumeRCTest07("test1");
        test07.next = test07; // 使用intrinsiccall MCCWriteNoDec（）代替
        test07.next.add();
        test07.add();
        if (test07.sum == 202) {
            System.out.println("ExpectResult");
        } else {
            System.out.println(test07.sum);
        }
    }

    void add() {
        sum = a + next.a;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n