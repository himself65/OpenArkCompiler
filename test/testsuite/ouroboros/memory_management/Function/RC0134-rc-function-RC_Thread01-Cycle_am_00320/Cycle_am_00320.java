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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Cycle_am_00320.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Cycle_a_00320 in RC测试-Cycle-01 to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_am_00320.java
 *- @ExecuteClass: Cycle_am_00320
 *- @ExecuteArgs:
 */

class ThreadRc_Cycle_am_00320 extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00320_A1 a1_main = new Cycle_a_00320_A1("a1_main");
        Cycle_a_00320_A4 a4_main = new Cycle_a_00320_A4("a4_main");
        Cycle_a_00320_A6 a6_main = new Cycle_a_00320_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00320_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00320_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00320_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a1_0 = a1_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;
        a4_main.a5_0 = a1_main.a2_0.a3_0.a5_0;
        a6_main.a4_0 = a4_main;

        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1551)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00320_A1 {
        Cycle_a_00320_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A1(String strObjectName) {
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

    class Cycle_a_00320_A2 {
        Cycle_a_00320_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A2(String strObjectName) {
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

    class Cycle_a_00320_A3 {
        Cycle_a_00320_A1 a1_0;
        Cycle_a_00320_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A3(String strObjectName) {
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

    class Cycle_a_00320_A4 {
        Cycle_a_00320_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A4(String strObjectName) {
            a5_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a5_0.a;
        }
    }

    class Cycle_a_00320_A5 {
        Cycle_a_00320_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A5(String strObjectName) {
            a6_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a;
        }
    }

    class Cycle_a_00320_A6 {
        Cycle_a_00320_A1 a1_0;
        Cycle_a_00320_A3 a3_0;
        Cycle_a_00320_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A6(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a4_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a + a4_0.a;
        }
    }
}


public class Cycle_am_00320 {
    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Cycle_am_00320 A1_Cycle_am_00320 = new ThreadRc_Cycle_am_00320();
        ThreadRc_Cycle_am_00320 A2_Cycle_am_00320 = new ThreadRc_Cycle_am_00320();
        ThreadRc_Cycle_am_00320 A3_Cycle_am_00320 = new ThreadRc_Cycle_am_00320();
        ThreadRc_Cycle_am_00320 A4_Cycle_am_00320 = new ThreadRc_Cycle_am_00320();
        ThreadRc_Cycle_am_00320 A5_Cycle_am_00320 = new ThreadRc_Cycle_am_00320();
        ThreadRc_Cycle_am_00320 A6_Cycle_am_00320 = new ThreadRc_Cycle_am_00320();

        A1_Cycle_am_00320.start();
        A2_Cycle_am_00320.start();
        A3_Cycle_am_00320.start();
        A4_Cycle_am_00320.start();
        A5_Cycle_am_00320.start();
        A6_Cycle_am_00320.start();

        try {
            A1_Cycle_am_00320.join();
            A2_Cycle_am_00320.join();
            A3_Cycle_am_00320.join();
            A4_Cycle_am_00320.join();
            A5_Cycle_am_00320.join();
            A6_Cycle_am_00320.join();

        } catch (InterruptedException e) {
        }
        if (A1_Cycle_am_00320.check() && A2_Cycle_am_00320.check() && A3_Cycle_am_00320.check() && A4_Cycle_am_00320.check() && A5_Cycle_am_00320.check() && A6_Cycle_am_00320.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n