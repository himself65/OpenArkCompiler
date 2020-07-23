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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread02/RC_Thread_15
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Multi Thread reads or writes static para.mofidfy from Cycle_B_2_00130
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_Thread_15.java
 *- @ExecuteClass: RC_Thread_15
 *- @ExecuteArgs:

 *
 */

import java.lang.Runtime;

class RC_Thread_15_1 extends Thread {
    public void run() {
        RC_Thread_15 rcth01 = new RC_Thread_15();
        try {
            rcth01.ModifyA1();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_15_2 extends Thread {
    public void run() {
        RC_Thread_15 rcth01 = new RC_Thread_15();
        try {
            rcth01.checkA4();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_15_3 extends Thread {
    public void run() {
        RC_Thread_15 rcth01 = new RC_Thread_15();
        try {
            rcth01.setA5();
        } catch (NullPointerException e) {

        }

    }
}

public class RC_Thread_15 {
    private static RC_Thread_15_A1 a1_main = null;
    private static RC_Thread_15_A5 a5_main = null;

    RC_Thread_15() {
        try {
            RC_Thread_15_A1 a1 = new RC_Thread_15_A1();
            a1.a2_0 = new RC_Thread_15_A2();
            a1.a2_0.a3_0 = new RC_Thread_15_A3();
            a1.a2_0.a3_0.a4_0 = new RC_Thread_15_A4();
            a1.a2_0.a3_0.a4_0.a1_0 = a1;
            RC_Thread_15_A5 a5 = new RC_Thread_15_A5();
            a1.a2_0.a3_0.a4_0.a6_0 = new RC_Thread_15_A6();
            a1.a2_0.a3_0.a4_0.a6_0.a5_0 = a5;
            a5.a3_0 = a1.a2_0.a3_0;
            a1_main = a1;
            a5_main = a5;
        } catch (NullPointerException e) {

        }
    }

    public static void main(String[] args) {
        cycle_pattern_wrapper();

        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

        System.out.println("ExpectResult");
    }

    private static void cycle_pattern_wrapper() {
        RC_Thread_15_A1 a1 = new RC_Thread_15_A1();
        a1.a2_0 = new RC_Thread_15_A2();
        a1.a2_0.a3_0 = new RC_Thread_15_A3();
        a1.a2_0.a3_0.a4_0 = new RC_Thread_15_A4();
        a1.a2_0.a3_0.a4_0.a1_0 = a1;
        RC_Thread_15_A5 a5 = new RC_Thread_15_A5();
        a1.a2_0.a3_0.a4_0.a6_0 = new RC_Thread_15_A6();
        a1.a2_0.a3_0.a4_0.a6_0.a5_0 = a5;
        a5.a3_0 = a1.a2_0.a3_0;
        a1 = null;
        a5 = null;
        Runtime.getRuntime().gc();
    }

    private static void rc_testcase_main_wrapper() {
        RC_Thread_15_1 t1 = new RC_Thread_15_1();
        RC_Thread_15_2 t2 = new RC_Thread_15_2();
        RC_Thread_15_3 t3 = new RC_Thread_15_3();
        t1.start();
        t2.start();
        t3.start();
        try {
            t1.join();
            t2.join();
            t3.join();
        } catch (InterruptedException e) {
        }
    }

    public void ModifyA1() {
        a1_main.a2_0.a3_0 = null;
        a1_main = null;
    }

    public void checkA4() {
        try {
            int[] arr = new int[2];
            arr[0] = a5_main.a3_0.a4_0.sum;
            arr[1] = a5_main.a3_0.a4_0.a;
        } catch (NullPointerException e) {

        }
    }

    public void setA5() {
        RC_Thread_15_A5 a5 = new RC_Thread_15_A5();
        a5 = this.a5_main;
        a5_main = null;
    }

    static class RC_Thread_15_A1 {
        RC_Thread_15_A2 a2_0;
        RC_Thread_15_A4 a4_0;
        int a;
        int sum;

        RC_Thread_15_A1() {
            a2_0 = null;
            a4_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }


    static class RC_Thread_15_A2 {
        RC_Thread_15_A3 a3_0;
        int a;
        int sum;

        RC_Thread_15_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }


    static class RC_Thread_15_A3 {
        RC_Thread_15_A4 a4_0;
        int a;
        int sum;

        RC_Thread_15_A3() {
            a4_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a4_0.a;
        }
    }


    static class RC_Thread_15_A4 {
        RC_Thread_15_A1 a1_0;
        RC_Thread_15_A6 a6_0;
        int a;
        int sum;

        RC_Thread_15_A4() {
            a1_0 = null;
            a6_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a + a6_0.a;
        }
    }

    static class RC_Thread_15_A5 {
        RC_Thread_15_A3 a3_0;
        int a;
        int sum;

        RC_Thread_15_A5() {
            a3_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    static class RC_Thread_15_A6 {
        RC_Thread_15_A5 a5_0;
        int a;
        int sum;

        RC_Thread_15_A6() {
            a5_0 = null;
            a = 6;
            sum = 0;
        }

        void add() {
            sum = a + a5_0.a;
        }
    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n