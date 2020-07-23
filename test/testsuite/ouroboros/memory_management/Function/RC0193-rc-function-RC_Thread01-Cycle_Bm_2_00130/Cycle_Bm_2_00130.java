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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Cycle_Bm_2_00130.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Cycle_B_2_00130 in RC测试-Cycle-00.vsd to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_Bm_2_00130.java
 *- @ExecuteClass: Cycle_Bm_2_00130
 *- @ExecuteArgs:
 */
class ThreadRc_Cycle_Bm_2_00130 extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_2_00130_A1 a1_0 = new Cycle_B_2_00130_A1();
        a1_0.a2_0 = new Cycle_B_2_00130_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_2_00130_A3();
        a1_0.a2_0.a3_0.a4_0 = new Cycle_B_2_00130_A4();
        a1_0.a2_0.a3_0.a4_0.a1_0 = a1_0;
        Cycle_B_2_00130_A5 a5_0 = new Cycle_B_2_00130_A5();
        a1_0.a2_0.a3_0.a4_0.a6_0 = new Cycle_B_2_00130_A6();
        a1_0.a2_0.a3_0.a4_0.a6_0.a5_0 = a5_0;
        a5_0.a3_0 = a1_0.a2_0.a3_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a1_0.a2_0.a3_0.a4_0.add();
        a5_0.add();
        a1_0.a2_0.a3_0.a4_0.a6_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a1_0.a2_0.a3_0.a4_0.sum + a5_0.sum + a1_0.a2_0.a3_0.a4_0.a6_0.sum);
        //System.out.println(nsum);

        if (nsum == 45)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_2_00130_A1 {
        Cycle_B_2_00130_A2 a2_0;
        Cycle_B_2_00130_A4 a4_0;
        int a;
        int sum;

        Cycle_B_2_00130_A1() {
            a2_0 = null;
            a4_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_2_00130_A2 {
        Cycle_B_2_00130_A3 a3_0;
        int a;
        int sum;

        Cycle_B_2_00130_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_2_00130_A3 {
        Cycle_B_2_00130_A4 a4_0;
        int a;
        int sum;

        Cycle_B_2_00130_A3() {
            a4_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_B_2_00130_A4 {
        Cycle_B_2_00130_A1 a1_0;
        Cycle_B_2_00130_A6 a6_0;
        int a;
        int sum;

        Cycle_B_2_00130_A4() {
            a1_0 = null;
            a6_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a + a6_0.a;
        }
    }

    class Cycle_B_2_00130_A5 {
        Cycle_B_2_00130_A3 a3_0;
        int a;
        int sum;

        Cycle_B_2_00130_A5() {
            a3_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_2_00130_A6 {
        Cycle_B_2_00130_A5 a5_0;
        int a;
        int sum;

        Cycle_B_2_00130_A6() {
            a5_0 = null;
            a = 6;
            sum = 0;
        }

        void add() {
            sum = a + a5_0.a;
        }
    }
}


public class Cycle_Bm_2_00130 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Cycle_Bm_2_00130 A1_Cycle_Bm_2_00130 = new ThreadRc_Cycle_Bm_2_00130();
        ThreadRc_Cycle_Bm_2_00130 A2_Cycle_Bm_2_00130 = new ThreadRc_Cycle_Bm_2_00130();
        ThreadRc_Cycle_Bm_2_00130 A3_Cycle_Bm_2_00130 = new ThreadRc_Cycle_Bm_2_00130();
        ThreadRc_Cycle_Bm_2_00130 A4_Cycle_Bm_2_00130 = new ThreadRc_Cycle_Bm_2_00130();
        ThreadRc_Cycle_Bm_2_00130 A5_Cycle_Bm_2_00130 = new ThreadRc_Cycle_Bm_2_00130();
        ThreadRc_Cycle_Bm_2_00130 A6_Cycle_Bm_2_00130 = new ThreadRc_Cycle_Bm_2_00130();

        A1_Cycle_Bm_2_00130.start();
        A2_Cycle_Bm_2_00130.start();
        A3_Cycle_Bm_2_00130.start();
        A4_Cycle_Bm_2_00130.start();
        A5_Cycle_Bm_2_00130.start();
        A6_Cycle_Bm_2_00130.start();

        try {
            A1_Cycle_Bm_2_00130.join();
            A2_Cycle_Bm_2_00130.join();
            A3_Cycle_Bm_2_00130.join();
            A4_Cycle_Bm_2_00130.join();
            A5_Cycle_Bm_2_00130.join();
            A6_Cycle_Bm_2_00130.join();

        } catch (InterruptedException e) {
        }
        if (A1_Cycle_Bm_2_00130.check() && A2_Cycle_Bm_2_00130.check() && A3_Cycle_Bm_2_00130.check() && A4_Cycle_Bm_2_00130.check() && A5_Cycle_Bm_2_00130.check() && A6_Cycle_Bm_2_00130.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n