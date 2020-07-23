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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Cycle_am_00450.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Cycle_a_00450 in RC测试-Cycle-01 to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_am_00450.java
 *- @ExecuteClass: Cycle_am_00450
 *- @ExecuteArgs:
 */

class ThreadRc_Cycle_am_00450 extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00450_A1 a1_main = new Cycle_a_00450_A1("a1_main");
        Cycle_a_00450_A6 a6_main = new Cycle_a_00450_A6("a6_main");
        Cycle_a_00450_A7 a7_main = new Cycle_a_00450_A7("a7_main");
        a1_main.a2_0 = new Cycle_a_00450_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00450_A3("a3_0");
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00450_A5("a5_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a5_0.a7_0 = a7_main;
        a7_main.a2_0 = a1_main.a2_0;

        a1_main.add();
        a6_main.add();
        a7_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();


        int result = a1_main.sum + a6_main.sum + a7_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1453)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00450_A1 {
        Cycle_a_00450_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A1(String strObjectName) {
            a2_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_a_00450_A2 {
        Cycle_a_00450_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A2(String strObjectName) {
            a3_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00450_A3 {
        Cycle_a_00450_A1 a1_0;
        Cycle_a_00450_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A3(String strObjectName) {
            a1_0 = null;
            a5_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a5_0.a;
        }
    }

    class Cycle_a_00450_A5 {
        Cycle_a_00450_A6 a6_0;
        Cycle_a_00450_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A5(String strObjectName) {
            a6_0 = null;
            a7_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a + a7_0.a;
        }
    }

    class Cycle_a_00450_A6 {
        Cycle_a_00450_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A6(String strObjectName) {
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00450_A7 {
        Cycle_a_00450_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A7(String strObjectName) {
            a2_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }
}


public class Cycle_am_00450 {
    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Cycle_am_00450 A1_Cycle_am_00450 = new ThreadRc_Cycle_am_00450();
        ThreadRc_Cycle_am_00450 A2_Cycle_am_00450 = new ThreadRc_Cycle_am_00450();
        ThreadRc_Cycle_am_00450 A3_Cycle_am_00450 = new ThreadRc_Cycle_am_00450();
        ThreadRc_Cycle_am_00450 A4_Cycle_am_00450 = new ThreadRc_Cycle_am_00450();
        ThreadRc_Cycle_am_00450 A5_Cycle_am_00450 = new ThreadRc_Cycle_am_00450();
        ThreadRc_Cycle_am_00450 A6_Cycle_am_00450 = new ThreadRc_Cycle_am_00450();

        A1_Cycle_am_00450.start();
        A2_Cycle_am_00450.start();
        A3_Cycle_am_00450.start();
        A4_Cycle_am_00450.start();
        A5_Cycle_am_00450.start();
        A6_Cycle_am_00450.start();

        try {
            A1_Cycle_am_00450.join();
            A2_Cycle_am_00450.join();
            A3_Cycle_am_00450.join();
            A4_Cycle_am_00450.join();
            A5_Cycle_am_00450.join();
            A6_Cycle_am_00450.join();

        } catch (InterruptedException e) {
        }
        if (A1_Cycle_am_00450.check() && A2_Cycle_am_00450.check() && A3_Cycle_am_00450.check() && A4_Cycle_am_00450.check() && A5_Cycle_am_00450.check() && A6_Cycle_am_00450.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n