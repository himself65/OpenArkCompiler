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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread02/RC_Thread_05.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Multi Thread reads or writes static para.mofidfy from Cycle_B_1_00180
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_Thread_05.java
 *- @ExecuteClass: RC_Thread_05
 *- @ExecuteArgs:

 *
 */

import java.lang.Runtime;

class RC_Thread_05_1 extends Thread {
    public void run() {
        RC_Thread_05 rcth01 = new RC_Thread_05();
        try {
            rcth01.setA1null();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_05_2 extends Thread {
    public void run() {
        RC_Thread_05 rcth01 = new RC_Thread_05();
        try {
            rcth01.setA4null();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_05_3 extends Thread {
    public void run() {
        RC_Thread_05 rcth01 = new RC_Thread_05();
        try {
            rcth01.setA5null();
        } catch (NullPointerException e) {

        }

    }
}

class RC_Thread_05_4 extends Thread {
    public void run() {
        RC_Thread_05 rcth01 = new RC_Thread_05();
        try {
            rcth01.setA1();
        } catch (NullPointerException e) {

        }

    }
}

class RC_Thread_05_5 extends Thread {
    public void run() {
        RC_Thread_05 rcth01 = new RC_Thread_05();
        try {
            rcth01.checkA3();
        } catch (NullPointerException e) {

        }

    }
}

class RC_Thread_05_6 extends Thread {
    public void run() {
        RC_Thread_05 rcth01 = new RC_Thread_05();
        try {
            rcth01.setA3_a(5);
        } catch (NullPointerException e) {

        }

    }
}

public class RC_Thread_05 {
    private volatile static RC_Thread_05_A1 a1_main = null;
    private volatile static RC_Thread_05_A4 a4_main = null;
    private volatile static RC_Thread_05_A5 a5_main = null;

    RC_Thread_05() {
        try {
            a1_main = new RC_Thread_05_A1();
            a1_main.a2_0 = new RC_Thread_05_A2();
            a1_main.a2_0.a3_0 = new RC_Thread_05_A3();
            a4_main = new RC_Thread_05_A4();
            a5_main = new RC_Thread_05_A5();
            a4_main.a1_0 = a1_main;
            a5_main.a1_0 = a1_main;
            a1_main.a2_0.a3_0.a1_0 = a1_main;
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
        a1_main = new RC_Thread_05_A1();
        a1_main.a2_0 = new RC_Thread_05_A2();
        a1_main.a2_0.a3_0 = new RC_Thread_05_A3();
        a4_main = new RC_Thread_05_A4();
        a5_main = new RC_Thread_05_A5();
        a4_main.a1_0 = a1_main;
        a5_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main = null;
        a4_main = null;
        a5_main = null;
        Runtime.getRuntime().gc();//单独构造一次Cycle_B_1_00180环，通过gc学习到环模式
    }

    private static void rc_testcase_main_wrapper() {
        RC_Thread_05_1 t1 = new RC_Thread_05_1();
        RC_Thread_05_2 t2 = new RC_Thread_05_2();
        RC_Thread_05_3 t3 = new RC_Thread_05_3();
        RC_Thread_05_4 t4 = new RC_Thread_05_4();
        RC_Thread_05_5 t5 = new RC_Thread_05_5();
        RC_Thread_05_6 t6 = new RC_Thread_05_6();
        t1.start();
        t2.start();
        t3.start();
        t4.start();
        t5.start();
        t6.start();
        try {
            t1.join();
            t2.join();
            t3.join();
            t4.join();
            t5.join();
            t6.join();

        } catch (InterruptedException e) {
        }
    }

    public void setA1null() {
        a1_main = null;
    }

    public void setA4null() {
        a4_main = null;
    }

    public void setA5null() {
        a5_main = null;
    }

    public void setA1() {
        try {
            this.a1_main = new RC_Thread_05_A1();
            a1_main.a2_0 = new RC_Thread_05_A2();
            a1_main.a2_0.a3_0 = new RC_Thread_05_A3();
        } catch (NullPointerException e) {

        }

    }

    public void checkA3() {
        try {
            int a = a1_main.a2_0.a3_0.a;
        } catch (NullPointerException e) {

        }

    }

    public void setA3_a(int a) {
        try {
            this.a1_main.a2_0.a3_0.a = a;
        } catch (NullPointerException e) {
        }
    }

    static class RC_Thread_05_A1 {
        volatile RC_Thread_05_A2 a2_0;
        int a;
        int sum;

        RC_Thread_05_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }


    static class RC_Thread_05_A2 {
        volatile RC_Thread_05_A3 a3_0;
        int a;
        int sum;

        RC_Thread_05_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }


    static class RC_Thread_05_A3 {
        volatile RC_Thread_05_A1 a1_0;
        int a;
        int sum;

        RC_Thread_05_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    static class RC_Thread_05_A4 {
        volatile RC_Thread_05_A1 a1_0;
        int a;
        int sum;

        RC_Thread_05_A4() {
            a1_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }


    static class RC_Thread_05_A5 {
        volatile RC_Thread_05_A1 a1_0;
        int a;
        int sum;

        RC_Thread_05_A5() {
            a1_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n