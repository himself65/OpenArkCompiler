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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest05
 *- @TestCaseName: LiverangeSubsumeRCTest05
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 *  -#step1: 构造是个同一个类对象的引用指针test1,test2,test3和test4，他们同时有实例变量同时指向一块内存地址b1;
 *  -#step2: b1的生命周期包含了这test1.b1_0,test2.b1_0和test3.b1_0的生命周期,没有包含test4.b1_0的生命周期。
 *  -#step3: test1.b1_0,test2.b1_0和test3.b1_0的incref和decRef都应该被优化掉都应该被优化掉；test4.b1_0的incref和decRef被保留。
 *  校验中间文件LiverangeSubsumeRCTest05.VtableImpl.mpl中无IncRef。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest05.java
 *- @ExecuteClass: LiverangeSubsumeRCTest05
 *- @ExecuteArgs:
 */

public class LiverangeSubsumeRCTest05 {
    public static void main(String[] args) {
        LiverangeSubsumeRCTest05_Node test1 = new LiverangeSubsumeRCTest05_Node("test1");
        LiverangeSubsumeRCTest05_Node test2 = new LiverangeSubsumeRCTest05_Node("test2");
        LiverangeSubsumeRCTest05_Node test3 = new LiverangeSubsumeRCTest05_Node("test3");
        LiverangeSubsumeRCTest05_Node test4 = new LiverangeSubsumeRCTest05_Node("test4");
        LiverangeSubsumeRCTest05_B1 b1 = new LiverangeSubsumeRCTest05_B1("b1");
        test1.b1_0 = b1;
        test2.b1_0 = b1;
        test3.b1_0 = b1;
        test4.b1_0 = b1;
        test1.b1_0.add();
        test2.b1_0.add();
        test3.b1_0.add();
        test4.b1_0.add();
        b1.add();
        int result = test1.sum + test2.sum + test3.sum + test4.sum + test4.b1_0.sum;
        if (result == 402) {
            System.out.println("ExpectResult");
        }
    }
}

class LiverangeSubsumeRCTest05_Node {
    LiverangeSubsumeRCTest05_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest05_Node(String strObjectName) {
        b1_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest05_B1 {
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest05_B1(String strObjectName) {
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
// ASSERT: scan ExpectResult\n