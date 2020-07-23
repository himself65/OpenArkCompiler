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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest08
 *- @TestCaseName: LiverangeSubsumeRCTest08
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作:两个对象成环的场景应该会被优化
 *  -#step1: test1与test1.next是两个不同的对象，占用不同的内存空间，而且test08.next.next的生命周期被test1包含，所以 test08.next的incref和dec ref都应该被优化掉。
 *  -#step2: 这样RC的自成环问题会被优化掉，该种场景仅通过RC就可以做到内存回收
 *  校验中间文件LiverangeSubsumeRCTest08.VtableImpl.mpl中无IncRef。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest08.java
 *- @ExecuteClass: LiverangeSubsumeRCTest08
 *- @ExecuteArgs:
 */
public class LiverangeSubsumeRCTest08 {
    LiverangeSubsumeRCTest08 next;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest08(String strObjectName) {
        next = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    public static void main(String[] args) {
        LiverangeSubsumeRCTest08 test1 = new LiverangeSubsumeRCTest08("test1");
        test1.next = new LiverangeSubsumeRCTest08("test2");
        test1.next.next = test1;
        test1.next.add();
        test1.next.next.add();
        test1.add();
        if (test1.sum == 202) {
            System.out.println("ExpectResult");
        } else {
            System.out.println(test1.sum);
        }
    }

    void add() {
        sum = a + next.a;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n