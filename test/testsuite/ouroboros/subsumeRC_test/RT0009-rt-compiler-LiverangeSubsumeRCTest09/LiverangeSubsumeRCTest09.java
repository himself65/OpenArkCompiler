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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest09
 *- @TestCaseName: LiverangeSubsumeRCTest09
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作:n(n>=3)个对象成环的场景应该会被优化
 *  -#step1: test9与它后面的next是总共5个不同的对象，并且自成环，而且并且后面的所有next的生命周期被test9包含，所以所有的.next的incref和dec ref都应该被优化掉。
 *  -#step2: 这样RC的自成环问题也会被优化掉，该种场景仅通过RC就可以做到内存回收
 *  校验中间文件LiverangeSubsumeRCTest09.VtableImpl.mpl中无IncRef
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest09.java
 *- @ExecuteClass: LiverangeSubsumeRCTest09
 *- @ExecuteArgs:
 */
public class LiverangeSubsumeRCTest09 {
    LiverangeSubsumeRCTest09 next;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest09(String strObjectName) {
        next = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    public static void main(String[] args) {
        LiverangeSubsumeRCTest09 test9 = new LiverangeSubsumeRCTest09("test9");
        test9.next = new LiverangeSubsumeRCTest09("test2");
        test9.next.next = new LiverangeSubsumeRCTest09("test3");
        test9.next.next.next = new LiverangeSubsumeRCTest09("test4");
        test9.next.next.next.next = new LiverangeSubsumeRCTest09("test5");
        test9.next.next.next.next = test9;
        test9.next.next.next.next.add();
        test9.add();
        if (test9.sum == 202) {
            System.out.println("ExpectResult");
        } else {
            System.out.println(test9.next.sum);
        }
    }

    void add() {
        sum = a + next.a;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n