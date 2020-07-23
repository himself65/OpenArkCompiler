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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread02/RC_Thread_18.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Multi Thread reads or writes static para.mofidfy from Cycle_a_00320
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_Thread_18.java
 *- @ExecuteClass: RC_Thread_18
 *- @ExecuteArgs:
 *
 */

import java.lang.Runtime;

class RC_Thread_18_1 extends Thread {
    public void run() {
        RC_Thread_18 rcth01 = new RC_Thread_18();
        try {
            rcth01.ModifyA3();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_18_2 extends Thread {
    public void run() {
        RC_Thread_18 rcth01 = new RC_Thread_18();
        try {
            rcth01.checkA3();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_18_3 extends Thread {
    public void run() {
        RC_Thread_18 rcth01 = new RC_Thread_18();
        try {
            rcth01.setA3null();
        } catch (NullPointerException e) {

        }

    }
}

public class RC_Thread_18 {
    private volatile static RC_Thread_18_A1 a1_main = null;
    private volatile static RC_Thread_18_A4 a4_main = null;
    private volatile static RC_Thread_18_A6 a6_main = null;
    private static RC_Thread_18_A2 a2 = null;

    RC_Thread_18() {
        try {
            RC_Thread_18_A1 a1 = new RC_Thread_18_A1("a1");
            RC_Thread_18_A4 a4 = new RC_Thread_18_A4("a4");
            RC_Thread_18_A6 a6 = new RC_Thread_18_A6("a6");
            a2 = new RC_Thread_18_A2("a2_0");
            a1.a2_0 = a2;
            a1.a2_0.a3_0 = new RC_Thread_18_A3("a3_0");
            a1.a2_0.a3_0.a1_0 = a1;
            a1.a2_0.a3_0.a5_0 = new RC_Thread_18_A5("a5_0");
            a1.a2_0.a3_0.a5_0.a6_0 = a6;
            a6.a1_0 = a1;
            a6.a3_0 = a1.a2_0.a3_0;
            a4.a5_0 = a1.a2_0.a3_0.a5_0;
            a6.a4_0 = a4;
            a1_main = a1;
            a4_main = a4;
            a6_main = a6;
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
        RC_Thread_18_A1 a1 = new RC_Thread_18_A1("a1");
        RC_Thread_18_A4 a4 = new RC_Thread_18_A4("a4");
        RC_Thread_18_A6 a6 = new RC_Thread_18_A6("a6");
        a2 = new RC_Thread_18_A2("a2_0");
        a1.a2_0 = a2;
        a1.a2_0.a3_0 = new RC_Thread_18_A3("a3_0");
        a1.a2_0.a3_0.a1_0 = a1;
        a1.a2_0.a3_0.a5_0 = new RC_Thread_18_A5("a5_0");
        a1.a2_0.a3_0.a5_0.a6_0 = a6;
        a6.a1_0 = a1;
        a6.a3_0 = a1.a2_0.a3_0;
        a4.a5_0 = a1.a2_0.a3_0.a5_0;
        a6.a4_0 = a4;
        a1 = null;
        a2 = null;
        a4 = null;
        a6 = null;
        Runtime.getRuntime().gc();
    }

    private static void rc_testcase_main_wrapper() {
        RC_Thread_18_1 t_00010 = new RC_Thread_18_1();
        RC_Thread_18_2 t_00020 = new RC_Thread_18_2();
        RC_Thread_18_3 t_00030 = new RC_Thread_18_3();
        t_00010.start();
        t_00020.start();
        t_00030.start();
        try {
            t_00010.join();
            t_00020.join();
            t_00030.join();

        } catch (InterruptedException e) {
        }

    }

    public void checkA3() {
        int[] arr = new int[2];
        try {
            arr[0] = a1_main.a2_0.a3_0.a;
            arr[1] = a1_main.a2_0.a3_0.sum;
        } catch (NullPointerException e) {

        }
    }

    public void ModifyA3() {
        try {
            a1_main.a2_0.a3_0 = new RC_Thread_18_A3("new-a3");
        } catch (NullPointerException e) {

        }
    }

    public void setA3null() {
        RC_Thread_18_A3 a3 = new RC_Thread_18_A3("test");
        try {
            a3 = this.a1_main.a2_0.a3_0;
            this.a1_main.a2_0.a3_0 = null;
        } catch (NullPointerException e) {

        }

    }

    static class RC_Thread_18_A1 {
        volatile RC_Thread_18_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_18_A1(String strObjectName) {
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

    static class RC_Thread_18_A2 {
        volatile RC_Thread_18_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_18_A2(String strObjectName) {
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

    static class RC_Thread_18_A3 {
        volatile RC_Thread_18_A1 a1_0;
        volatile RC_Thread_18_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_18_A3(String strObjectName) {
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


    static class RC_Thread_18_A4 {
        volatile RC_Thread_18_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_18_A4(String strObjectName) {
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

    static class RC_Thread_18_A5 {
        volatile RC_Thread_18_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_18_A5(String strObjectName) {
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

    static class RC_Thread_18_A6 {
        volatile RC_Thread_18_A1 a1_0;
        volatile RC_Thread_18_A3 a3_0;
        volatile RC_Thread_18_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_18_A6(String strObjectName) {
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


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n