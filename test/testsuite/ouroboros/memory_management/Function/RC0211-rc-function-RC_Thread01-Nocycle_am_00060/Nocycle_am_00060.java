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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Nocycle_am_00060.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Nocycle_a_00060 in RC测试-No-Cycle-00.vsd to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_am_00060.java
 *- @ExecuteClass: Nocycle_am_00060
 *- @ExecuteArgs:
 */
class ThreadRc_00060 extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00060_A1 a1_main = new Nocycle_a_00060_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00060_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00060_B2("b2_0");
        a1_main.b2_0.c1_0 = new Nocycle_a_00060_C1("c1_0");
        a1_main.b2_0.c2_0 = new Nocycle_a_00060_C2("c2_0");
        a1_main.b2_0.c3_0 = new Nocycle_a_00060_C3("c3_0");
        a1_main.b3_0 = new Nocycle_a_00060_B3("b3_0");
        a1_main.b4_0 = new Nocycle_a_00060_B4("b4_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a1_main.b4_0.add();
        a1_main.b2_0.c1_0.add();
        a1_main.b2_0.c2_0.add();
        a1_main.b2_0.c3_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum+a1_main.b4_0.sum+a1_main.b2_0.c1_0.sum+a1_main.b2_0.c2_0.sum+a1_main.b2_0.c3_0.sum);

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a1_main.b4_0.sum + a1_main.b2_0.c1_0.sum + a1_main.b2_0.c2_0.sum + a1_main.b2_0.c3_0.sum;
        //System.out.println("RC-Testing_Result_Thread1="+result);
        if (result == 5047)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00060_A1 {
        Nocycle_a_00060_B1 b1_0;
        Nocycle_a_00060_B2 b2_0;
        Nocycle_a_00060_B3 b3_0;
        Nocycle_a_00060_B4 b4_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            b3_0 = null;
            b4_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a + b3_0.a + b4_0.a;
        }
    }

    class Nocycle_a_00060_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00060_B2 {
        Nocycle_a_00060_C1 c1_0;
        Nocycle_a_00060_C2 c2_0;
        Nocycle_a_00060_C3 c3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_B2(String strObjectName) {
            c1_0 = null;
            c2_0 = null;
            c3_0 = null;
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a + c2_0.a + c3_0.a;
        }
    }

    class Nocycle_a_00060_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00060_B4 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_B4(String strObjectName) {
            a = 204;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00060_C1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_C1(String strObjectName) {
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00060_C2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_C2(String strObjectName) {
            a = 302;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00060_C3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00060_C3(String strObjectName) {
            a = 303;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

public class Nocycle_am_00060 {
    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_00060 A1_00060 = new ThreadRc_00060();
        ThreadRc_00060 A2_00060 = new ThreadRc_00060();
        ThreadRc_00060 A3_00060 = new ThreadRc_00060();
        ThreadRc_00060 A4_00060 = new ThreadRc_00060();
        ThreadRc_00060 A5_00060 = new ThreadRc_00060();
        ThreadRc_00060 A6_00060 = new ThreadRc_00060();
        ThreadRc_00060 A7_00060 = new ThreadRc_00060();
        ThreadRc_00060 A8_00060 = new ThreadRc_00060();
        ThreadRc_00060 A9_00060 = new ThreadRc_00060();
        ThreadRc_00060 A10_00060 = new ThreadRc_00060();

        A1_00060.start();
        A2_00060.start();
        A3_00060.start();
        A4_00060.start();
        A5_00060.start();
        A6_00060.start();
        A7_00060.start();
        A8_00060.start();
        A9_00060.start();
        A10_00060.start();

        try {
            A1_00060.join();
            A2_00060.join();
            A3_00060.join();
            A4_00060.join();
            A5_00060.join();
            A6_00060.join();
            A7_00060.join();
            A8_00060.join();
            A9_00060.join();
            A10_00060.join();
        } catch (InterruptedException e) {
        }
        if (A1_00060.check() && A2_00060.check() && A3_00060.check() && A4_00060.check() && A5_00060.check() && A6_00060.check() && A7_00060.check() && A8_00060.check() && A9_00060.check() && A10_00060.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n