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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Nocycle_am_00100.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Nocycle_a_00100 in RC测试-No-Cycle-00.vsd to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_am_00100.java
 *- @ExecuteClass: Nocycle_am_00100
 *- @ExecuteArgs:
 */
class ThreadRc_00100 extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00100_A1 a1_main = new Nocycle_a_00100_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00100_B1("b1_0");
        a1_main.b1_0.c1_0 = new Nocycle_a_00100_C1("c1_0");
        a1_main.b1_0.c1_0.d1_0 = new Nocycle_a_00100_D1("d1_0");
        a1_main.b1_0.c1_0.d1_0.e1_0 = new Nocycle_a_00100_E1("e1_0");
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0 = new Nocycle_a_00100_F1("f1_0");
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0 = new Nocycle_a_00100_G1("g1_0");
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0 = new Nocycle_a_00100_H1("h1_0");
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0 = new Nocycle_a_00100_I1("i1_0");
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0 = new Nocycle_a_00100_J1("j1_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b1_0.c1_0.add();
        a1_main.b1_0.c1_0.d1_0.add();
        a1_main.b1_0.c1_0.d1_0.e1_0.add();
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.add();
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.add();
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.add();
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.add();
        a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0.add();
//         System.out.printf("a=%d\n",a1_main.sum);
//         System.out.printf("b=%d\n",a1_main.b1_0.sum);
//         System.out.printf("c=%d\n",a1_main.b1_0.c1_0.sum);
//         System.out.printf("d=%d\n",a1_main.b1_0.c1_0.d1_0.sum);
//         System.out.printf("e=%d\n",a1_main.b1_0.c1_0.d1_0.e1_0.sum);
//         System.out.printf("f=%d\n",a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.sum);
//         System.out.printf("g=%d\n",a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.sum);
//         System.out.printf("h=%d\n",a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.sum);
//         System.out.printf("i=%d\n",a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.sum);
//         System.out.printf("j=%d\n",a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0.sum);
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b1_0.c1_0.sum+a1_main.b1_0.c1_0.d1_0.sum+a1_main.b1_0.c1_0.d1_0.e1_0.sum+a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.sum+a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.sum+a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.sum+a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.sum+a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0.sum);

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b1_0.c1_0.sum + a1_main.b1_0.c1_0.d1_0.sum + a1_main.b1_0.c1_0.d1_0.e1_0.sum + a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.sum + a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.sum + a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.sum + a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.sum + a1_main.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0.sum;

        //System.out.println("RC-Testing_Result="+result);

        if (result == 11920)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00100_A1 {
        Nocycle_a_00100_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_A1(String strObjectName) {
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

    class Nocycle_a_00100_B1 {
        Nocycle_a_00100_C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_B1(String strObjectName) {
            c1_0 = null;
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a;
        }
    }

    class Nocycle_a_00100_C1 {
        Nocycle_a_00100_D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_C1(String strObjectName) {
            d1_0 = null;
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + d1_0.a;
        }
    }

    class Nocycle_a_00100_D1 {
        Nocycle_a_00100_E1 e1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_D1(String strObjectName) {
            e1_0 = null;
            a = 401;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
        }

        void add() {
            sum = a + e1_0.a;
        }
    }

    class Nocycle_a_00100_E1 {
        Nocycle_a_00100_F1 f1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_E1(String strObjectName) {
            f1_0 = null;
            a = 501;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_E1_"+strObjectName);
        }

        void add() {
            sum = a + f1_0.a;
        }
    }

    class Nocycle_a_00100_F1 {
        Nocycle_a_00100_G1 g1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_F1(String strObjectName) {
            g1_0 = null;
            a = 601;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_F1_"+strObjectName);
        }

        void add() {
            sum = a + g1_0.a;
        }
    }

    class Nocycle_a_00100_G1 {
        Nocycle_a_00100_H1 h1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_G1(String strObjectName) {
            h1_0 = null;
            a = 701;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_G1_"+strObjectName);
        }

        void add() {
            sum = a + h1_0.a;
        }
    }

    class Nocycle_a_00100_H1 {
        Nocycle_a_00100_I1 i1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_H1(String strObjectName) {
            i1_0 = null;
            a = 801;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_H1_"+strObjectName);
        }

        void add() {
            sum = a + i1_0.a;
        }
    }

    class Nocycle_a_00100_I1 {
        Nocycle_a_00100_J1 j1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_I1(String strObjectName) {
            j1_0 = null;
            a = 901;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_I1_"+strObjectName);
        }

        void add() {
            sum = a + j1_0.a;
        }
    }

    class Nocycle_a_00100_J1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00100_J1(String strObjectName) {
            a = 1001;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_J1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}


public class Nocycle_am_00100 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_00100 A1_00100 = new ThreadRc_00100();
        ThreadRc_00100 A2_00100 = new ThreadRc_00100();
        ThreadRc_00100 A3_00100 = new ThreadRc_00100();
        ThreadRc_00100 A4_00100 = new ThreadRc_00100();
        ThreadRc_00100 A5_00100 = new ThreadRc_00100();
        ThreadRc_00100 A6_00100 = new ThreadRc_00100();

        A1_00100.start();
        A2_00100.start();
        A3_00100.start();
        A4_00100.start();
        A5_00100.start();
        A6_00100.start();

        try {
            A1_00100.join();
            A2_00100.join();
            A3_00100.join();
            A4_00100.join();
            A5_00100.join();
            A6_00100.join();

        } catch (InterruptedException e) {
        }
        if (A1_00100.check() && A2_00100.check() && A3_00100.check() && A4_00100.check() && A5_00100.check() && A6_00100.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n