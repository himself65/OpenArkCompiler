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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest04
 *- @TestCaseName: LiverangeSubsumeRCTest04
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 *  -#step1: 构造是个类对象A1、A2、A3···A10，他们同时有实例变量同时指向一块内存地址b1;
 *  -#step2: b1的生命周期包含了这十个成员变量(A1···A10)的生命周期。代码51行--66行不再出现实例变量
 *  -#step3: A1到A10的十个实例变量的incref和decRef都应该被优化掉都应该被优化掉。
 *  校验中间文件LiverangeSubsumeRCTest04.VtableImpl.mpl无IncRef。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest04.java
 *- @ExecuteClass: LiverangeSubsumeRCTest04
 *- @ExecuteArgs:
 */

public class LiverangeSubsumeRCTest04 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
    }

    private static void rc_testcase_main_wrapper() {
        LiverangeSubsumeRCTest04_A1 a1_main = new LiverangeSubsumeRCTest04_A1("a1_main");
        LiverangeSubsumeRCTest04_A2 a2_main = new LiverangeSubsumeRCTest04_A2("a2_main");
        LiverangeSubsumeRCTest04_A3 a3_main = new LiverangeSubsumeRCTest04_A3("a3_main");
        LiverangeSubsumeRCTest04_A4 a4_main = new LiverangeSubsumeRCTest04_A4("a4_main");
        LiverangeSubsumeRCTest04_A5 a5_main = new LiverangeSubsumeRCTest04_A5("a5_main");
        LiverangeSubsumeRCTest04_A6 a6_main = new LiverangeSubsumeRCTest04_A6("a6_main");
        LiverangeSubsumeRCTest04_A7 a7_main = new LiverangeSubsumeRCTest04_A7("a7_main");
        LiverangeSubsumeRCTest04_A8 a8_main = new LiverangeSubsumeRCTest04_A8("a8_main");
        LiverangeSubsumeRCTest04_A9 a9_main = new LiverangeSubsumeRCTest04_A9("a9_main");
        LiverangeSubsumeRCTest04_A10 a10_main = new LiverangeSubsumeRCTest04_A10("a10_main");
        LiverangeSubsumeRCTest04_B1 b1 = new LiverangeSubsumeRCTest04_B1("b1_0");
        a1_main.b1_0 = b1;
        a2_main.b1_0 = b1;
        a3_main.b1_0 = b1;
        a4_main.b1_0 = b1;
        a5_main.b1_0 = b1;
        a6_main.b1_0 = b1;
        a7_main.b1_0 = b1;
        a8_main.b1_0 = b1;
        a9_main.b1_0 = b1;
        a10_main.b1_0 = b1;
        a1_main.add();
        a2_main.add();
        a3_main.add();
        a4_main.add();
        a5_main.add();
        a6_main.add();
        a7_main.add();
        a8_main.add();
        a9_main.add();
        a10_main.add();
        b1.add();
        int result = a1_main.sum + a2_main.sum + a3_main.sum + a4_main.sum + a5_main.sum + a6_main.sum + a7_main.sum + a8_main.sum + a9_main.sum + a10_main.sum + b1.sum;
        if (result == 3467)
            System.out.println("ExpectResult");
    }
}

class LiverangeSubsumeRCTest04_A1 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A1(String strObjectName) {
        b1_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest04_A2 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A2(String strObjectName) {
        b1_0 = null;
        a = 102;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}


class LiverangeSubsumeRCTest04_A3 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A3(String strObjectName) {
        b1_0 = null;
        a = 103;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest04_A4 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A4(String strObjectName) {
        b1_0 = null;
        a = 104;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest04_A5 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A5(String strObjectName) {
        b1_0 = null;
        a = 105;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}


class LiverangeSubsumeRCTest04_A6 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A6(String strObjectName) {
        b1_0 = null;
        a = 106;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest04_A7 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A7(String strObjectName) {
        b1_0 = null;
        a = 107;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest04_A8 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A8(String strObjectName) {
        b1_0 = null;
        a = 108;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest04_A9 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A9(String strObjectName) {
        b1_0 = null;
        a = 109;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A9_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class LiverangeSubsumeRCTest04_A10 {
    LiverangeSubsumeRCTest04_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_A10(String strObjectName) {
        b1_0 = null;
        a = 110;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A10_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a;
    }
}


class LiverangeSubsumeRCTest04_B1 {
    int a;
    int sum;
    String strObjectName;

    LiverangeSubsumeRCTest04_B1(String strObjectName) {
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