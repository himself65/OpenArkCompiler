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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/RCUnownedRefUncycleThread2.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change RCUnownedRefUncycle02 to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RCUnownedRefUncycleThread2.java
 *- @ExecuteClass: RCUnownedRefUncycleThread2
 *- @ExecuteArgs:
 */

import com.huawei.ark.annotation.Unowned;

class ThreadRc_00180 extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00180_A1 a1_main = new Nocycle_a_00180_A1("a1_main");
        Nocycle_a_00180_A2 a2_main = new Nocycle_a_00180_A2("a2_main");
        Nocycle_a_00180_A3 a3_main = new Nocycle_a_00180_A3("a3_main");
        Nocycle_a_00180_A4 a4_main = new Nocycle_a_00180_A4("a4_main");
        a1_main.b1_0 = new Nocycle_a_00180_B1("b1_0");
        a1_main.d1_0 = new Nocycle_a_00180_D1("d1_0");
        a1_main.b1_0.d2_0 = new Nocycle_a_00180_D2("d2_0");

        a2_main.b2_0 = new Nocycle_a_00180_B2("b2_0");
        a2_main.b2_0.c1_0 = new Nocycle_a_00180_C1("c1_0");
        a2_main.b2_0.d1_0 = a1_main.d1_0;
        a2_main.b2_0.d2_0 = a1_main.b1_0.d2_0;
        a2_main.b2_0.d3_0 = new Nocycle_a_00180_D3("d3_0");
        a2_main.b2_0.c1_0.d1_0 = a1_main.d1_0;

        a3_main.b2_0 = a2_main.b2_0;
        a3_main.b2_0.c1_0 = a2_main.b2_0.c1_0;
        a3_main.b2_0.c1_0.d1_0 = a1_main.d1_0;
        a3_main.b2_0.d1_0 = a1_main.d1_0;
        a3_main.b2_0.d2_0 = a1_main.b1_0.d2_0;
        a3_main.b2_0.d3_0 = a2_main.b2_0.d3_0;

        a3_main.c2_0 = new Nocycle_a_00180_C2("c2_0");
        a3_main.c2_0.d2_0 = a1_main.b1_0.d2_0;
        a3_main.c2_0.d3_0 = a2_main.b2_0.d3_0;

        a4_main.b3_0 = new Nocycle_a_00180_B3("b3_0");
        a4_main.b3_0.c1_0 = a2_main.b2_0.c1_0;
        a4_main.b3_0.c1_0.d1_0 = a1_main.d1_0;
        a4_main.c2_0 = a3_main.c2_0;
        a4_main.c2_0.d2_0 = a1_main.b1_0.d2_0;
        a4_main.c2_0.d3_0 = a2_main.b2_0.d3_0;

        a1_main.add();
        a2_main.add();
        a3_main.add();
        a4_main.add();
        a1_main.b1_0.add();
        a1_main.d1_0.add();
        a1_main.b1_0.d2_0.add();

        a2_main.b2_0.add();
        a2_main.b2_0.c1_0.add();
        a2_main.b2_0.d1_0.add();
        a2_main.b2_0.d2_0.add();
        a2_main.b2_0.d3_0.add();
        a2_main.b2_0.c1_0.d1_0.add();

        a3_main.b2_0.add();
        a3_main.b2_0.c1_0.add();
        a3_main.b2_0.c1_0.d1_0.add();
        a3_main.b2_0.d1_0.add();
        a3_main.b2_0.d2_0.add();
        a3_main.b2_0.d3_0.add();

        a3_main.c2_0.add();
        a3_main.c2_0.d2_0.add();
        a3_main.c2_0.d3_0.add();

        a4_main.b3_0.add();
        a4_main.b3_0.c1_0.add();
        a4_main.b3_0.c1_0.d1_0.add();
        a4_main.c2_0.add();
        a4_main.c2_0.d2_0.add();
        a4_main.c2_0.d3_0.add();

        int result = a1_main.sum + a2_main.sum + a3_main.sum + a4_main.sum + a1_main.b1_0.sum + a2_main.b2_0.sum + a4_main.b3_0.sum + a3_main.c2_0.sum + a3_main.c2_0.d2_0.sum + a3_main.c2_0.d3_0.sum + a1_main.d1_0.sum + a2_main.b2_0.c1_0.sum;

        if (result == 9260)
            checkout = true;
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00180_A1 {
        Nocycle_a_00180_B1 b1_0;
        Nocycle_a_00180_D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_A1(String strObjectName) {
            b1_0 = null;
            d1_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + d1_0.a;
        }
    }

    class Nocycle_a_00180_A2 {
        Nocycle_a_00180_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_A2(String strObjectName) {
            b2_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + b2_0.a;
        }
    }

    class Nocycle_a_00180_A3 {
        @Unowned
        Nocycle_a_00180_B2 b2_0;
        Nocycle_a_00180_C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_A3(String strObjectName) {
            b2_0 = null;
            c2_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + b2_0.a + c2_0.a;
        }
    }

    class Nocycle_a_00180_A4 {
        Nocycle_a_00180_B3 b3_0;
        @Unowned
        Nocycle_a_00180_C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_A4(String strObjectName) {
            b3_0 = null;
            c2_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + b3_0.a + c2_0.a;
        }
    }

    class Nocycle_a_00180_B1 {
        Nocycle_a_00180_D2 d2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_B1(String strObjectName) {
            d2_0 = null;
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + d2_0.a;
        }
    }

    class Nocycle_a_00180_B2 {
        Nocycle_a_00180_C1 c1_0;
        @Unowned
        Nocycle_a_00180_D1 d1_0;
        @Unowned
        Nocycle_a_00180_D2 d2_0;
        Nocycle_a_00180_D3 d3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_B2(String strObjectName) {
            c1_0 = null;
            d1_0 = null;
            d2_0 = null;
            d3_0 = null;
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a + d1_0.a + d2_0.a + d3_0.a;
        }
    }

    class Nocycle_a_00180_B3 {
        @Unowned
        Nocycle_a_00180_C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_B3(String strObjectName) {
            c1_0 = null;
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a;
        }
    }

    class Nocycle_a_00180_C1 {
        @Unowned
        Nocycle_a_00180_D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_C1(String strObjectName) {
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

    class Nocycle_a_00180_C2 {
        @Unowned
        Nocycle_a_00180_D2 d2_0;
        @Unowned
        Nocycle_a_00180_D3 d3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_C2(String strObjectName) {
            d2_0 = null;
            d3_0 = null;
            a = 302;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
        }

        void add() {
            sum = a + d2_0.a + d3_0.a;
        }
    }

    class Nocycle_a_00180_D1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_D1(String strObjectName) {
            a = 401;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00180_D2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_D2(String strObjectName) {
            a = 402;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00180_D3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00180_D3(String strObjectName) {
            a = 403;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}


public class RCUnownedRefUncycleThread2 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_00180 A1_00180 = new ThreadRc_00180();
        ThreadRc_00180 A2_00180 = new ThreadRc_00180();
        ThreadRc_00180 A3_00180 = new ThreadRc_00180();
        ThreadRc_00180 A4_00180 = new ThreadRc_00180();
        ThreadRc_00180 A5_00180 = new ThreadRc_00180();
        ThreadRc_00180 A6_00180 = new ThreadRc_00180();

        A1_00180.start();
        A2_00180.start();
        A3_00180.start();
        A4_00180.start();
        A5_00180.start();
        A6_00180.start();

        try {
            A1_00180.join();
            A2_00180.join();
            A3_00180.join();
            A4_00180.join();
            A5_00180.join();
            A6_00180.join();

        } catch (InterruptedException e) {
        }
        if (A1_00180.check() && A2_00180.check() && A3_00180.check() && A4_00180.check() && A5_00180.check() && A6_00180.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n