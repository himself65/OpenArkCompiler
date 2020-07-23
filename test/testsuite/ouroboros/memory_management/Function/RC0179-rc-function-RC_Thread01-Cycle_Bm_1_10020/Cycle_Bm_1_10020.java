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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Cycle_Bm_1_10020.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Cycle_B_1_10020 in RC测试-Cycle-00.vsd to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_Bm_1_10020.java
 *- @ExecuteClass: Cycle_Bm_1_10020
 *- @ExecuteArgs:
 */
class ThreadRc_Cycle_Bm_1_10020 extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_10020_A1 a1_0 = new Cycle_B_1_10020_A1();
        a1_0.a2_0 = new Cycle_B_1_10020_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_10020_A3();
        Cycle_B_1_10020_A4 a4_0 = new Cycle_B_1_10020_A4();
        Cycle_B_1_10020_A5 a5_0 = new Cycle_B_1_10020_A5();
        a4_0.a1_0 = a1_0;
        a5_0.a1_0 = a1_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        a5_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum + a5_0.sum);
        Cycle_B_1_10020_2A1 a1_20 = new Cycle_B_1_10020_2A1();
        a1_20.a2_0 = new Cycle_B_1_10020_2A2();
        a1_20.a2_0.a3_0 = new Cycle_B_1_10020_2A3();
        Cycle_B_1_10020_2A4 a4_20 = new Cycle_B_1_10020_2A4();
        Cycle_B_1_10020_2A5 a5_20 = new Cycle_B_1_10020_2A5();
        a1_20.a4_0 = a4_20;
        a1_20.a5_0 = a5_20;
        a1_20.a2_0.a3_0.a1_0 = a1_20;
        a1_20.add();
        a1_20.a2_0.add();
        a1_20.a2_0.a3_0.add();
        a4_20.add();
        a5_20.add();
        int nsum1 = (a1_20.sum + a1_20.a2_0.sum + a1_20.a2_0.a3_0.sum + a4_20.sum + a5_20.sum);
        nsum += nsum1;

        if (nsum == 61)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_10020_A1 {
        Cycle_B_1_10020_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_10020_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_10020_A2 {
        Cycle_B_1_10020_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_10020_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_10020_A3 {
        Cycle_B_1_10020_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_10020_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_10020_A4 {
        Cycle_B_1_10020_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_10020_A4() {
            a1_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_10020_A5 {
        Cycle_B_1_10020_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_10020_A5() {
            a1_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_10020_2A1 {
        Cycle_B_1_10020_2A2 a2_0;
        Cycle_B_1_10020_2A4 a4_0;
        Cycle_B_1_10020_2A5 a5_0;
        int a;
        int sum;

        Cycle_B_1_10020_2A1() {
            a2_0 = null;
            a4_0 = null;
            a5_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_10020_2A2 {
        Cycle_B_1_10020_2A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_10020_2A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_10020_2A3 {
        Cycle_B_1_10020_2A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_10020_2A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_10020_2A4 {
        int a;
        int sum;

        Cycle_B_1_10020_2A4() {
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + 6;
        }
    }

    class Cycle_B_1_10020_2A5 {
        int a;
        int sum;

        Cycle_B_1_10020_2A5() {
            a = 7;
            sum = 0;
        }

        void add() {
            sum = a + 8;
        }
    }
}


public class Cycle_Bm_1_10020 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Cycle_Bm_1_10020 A1_Cycle_Bm_1_10020 = new ThreadRc_Cycle_Bm_1_10020();
        ThreadRc_Cycle_Bm_1_10020 A2_Cycle_Bm_1_10020 = new ThreadRc_Cycle_Bm_1_10020();
        ThreadRc_Cycle_Bm_1_10020 A3_Cycle_Bm_1_10020 = new ThreadRc_Cycle_Bm_1_10020();
        ThreadRc_Cycle_Bm_1_10020 A4_Cycle_Bm_1_10020 = new ThreadRc_Cycle_Bm_1_10020();
        ThreadRc_Cycle_Bm_1_10020 A5_Cycle_Bm_1_10020 = new ThreadRc_Cycle_Bm_1_10020();
        ThreadRc_Cycle_Bm_1_10020 A6_Cycle_Bm_1_10020 = new ThreadRc_Cycle_Bm_1_10020();

        A1_Cycle_Bm_1_10020.start();
        A2_Cycle_Bm_1_10020.start();
        A3_Cycle_Bm_1_10020.start();
        A4_Cycle_Bm_1_10020.start();
        A5_Cycle_Bm_1_10020.start();
        A6_Cycle_Bm_1_10020.start();

        try {
            A1_Cycle_Bm_1_10020.join();
            A2_Cycle_Bm_1_10020.join();
            A3_Cycle_Bm_1_10020.join();
            A4_Cycle_Bm_1_10020.join();
            A5_Cycle_Bm_1_10020.join();
            A6_Cycle_Bm_1_10020.join();

        } catch (InterruptedException e) {
        }
        if (A1_Cycle_Bm_1_10020.check() && A2_Cycle_Bm_1_10020.check() && A3_Cycle_Bm_1_10020.check() && A4_Cycle_Bm_1_10020.check() && A5_Cycle_Bm_1_10020.check() && A6_Cycle_Bm_1_10020.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n