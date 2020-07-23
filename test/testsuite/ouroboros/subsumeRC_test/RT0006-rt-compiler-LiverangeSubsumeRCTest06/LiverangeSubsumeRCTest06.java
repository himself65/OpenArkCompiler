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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest06
 *- @TestCaseName: LiverangeSubsumeRCTest06
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 *  -#step1: 构造是个同一个类对象的引用指针test1,test2,test3和test4，他们同时指向一块内存地址，并且他们的实例变量b1_0同时指向一块内存地址b1;
 *  -#step2: test1的生命周期包含了test2,test3 和test4,所以test2,test3和test4的incref和dec ref都应该被优化掉。
 *  -#step3: b1指针的生命周期包含了四个b1_0的生命周期，四个b1_0的incref，decref应该被优化掉
 *  校验中间文件LiverangeSubsumeRCTest06.VtableImpl.mpl无IncRef
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest06.java
 *- @ExecuteClass: LiverangeSubsumeRCTest06
 *- @ExecuteArgs:
 */

public class LiverangeSubsumeRCTest06 {
    public static void main(String[] args) {
        LiverangeSubsumeRCTest06_Node test1 = new LiverangeSubsumeRCTest06_Node("test1");
        LiverangeSubsumeRCTest06_Node test2 = test1;
        LiverangeSubsumeRCTest06_Node test3 = test1;
        LiverangeSubsumeRCTest06_Node test4 = test1;
        LiverangeSubsumeRCTest06_B1 b1 = new LiverangeSubsumeRCTest06_B1("b1");
        test2.b1_0 = b1;
        test2.b1_0.add();
        test3.b1_0 = b1;
        test3.b1_0.add();
        test4.b1_0 = b1;
        test4.b1_0.add();
        test1.b1_0 = b1;
        test1.b1_0.add();
        int result = test1.sum + b1.sum;
        if (result == 402) {
            System.out.println("ExpectResult");
        }
    }
}

class LiverangeSubsumeRCTest06_Node {
    LiverangeSubsumeRCTest06_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest06_Node(String strObjectName) {
        b1_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest06_B1 {
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest06_B1(String strObjectName) {
        a = 201;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n