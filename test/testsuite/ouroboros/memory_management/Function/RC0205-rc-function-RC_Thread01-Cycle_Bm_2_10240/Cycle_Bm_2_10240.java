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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Cycle_Bm_2_10240.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Make Cycle_B_2_10240 together to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\nExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_Bm_2_10240.java
 *- @ExecuteClass: Cycle_Bm_2_10240
 *- @ExecuteArgs:
 */
class ThreadRc_Cycle_Bm_2_10240 extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_2_00240_A1 a1_main = new Cycle_B_2_00240_A1("a1_main");
        a1_main.a2_0 = new Cycle_B_2_00240_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_B_2_00240_A3("a3_0");
        a1_main.a2_0.a4_0 = new Cycle_B_2_00240_A4("a4_0");
        a1_main.a2_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a4_0.a2_0 = a1_main.a2_0;
        a1_main.a2_0.a4_0.a1_0 = a1_main;


        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a4_0.add();


        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a4_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        Cycle_B_2_00240_B1 b1_0 = new Cycle_B_2_00240_B1();
        b1_0.a3_0 = new Cycle_B_2_00240_A3("a3_b1");
        b1_0.b2_0 = new Cycle_B_2_00240_B2();
        b1_0.c2_0 = new Nocycle_B_2_00240_C2("c2_b1");
        b1_0.b2_0.b1_0 = b1_0;
        b1_0.add();
        b1_0.b2_0.add();

        int nsum = (b1_0.sum + b1_0.b2_0.sum);
        // System.out.println(nsum);


        Nocycle_B_2_00240_C1 c1_main = new Nocycle_B_2_00240_C1("c1_main");
        c1_main.c2_0 = new Nocycle_B_2_00240_C2("c2_0");
        c1_main.add();
        c1_main.c2_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",c1_main.sum+c1_main.b1_0.sum);
        int result2 = c1_main.sum + c1_main.c2_0.sum;
        //System.out.println("RC-Testing_Result="+result2);

        if (result == 1124 && nsum == 310 && result2 == 704)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_2_00240_A1 {
        Cycle_B_2_00240_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_B_2_00240_A1(String strObjectName) {
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

    class Cycle_B_2_00240_A2 {
        Cycle_B_2_00240_A1 a1_0;
        Cycle_B_2_00240_A3 a3_0;
        Cycle_B_2_00240_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_B_2_00240_A2(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a4_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_B_2_00240_A3 {
        Cycle_B_2_00240_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_B_2_00240_A3(String strObjectName) {
            a1_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_2_00240_A4 {
        Cycle_B_2_00240_A1 a1_0;
        Cycle_B_2_00240_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_B_2_00240_A4(String strObjectName) {
            a2_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a2_0.a;
        }
    }

    class Cycle_B_2_00240_B1 {
        Cycle_B_2_00240_A3 a3_0;
        Cycle_B_2_00240_B2 b2_0;
        Nocycle_B_2_00240_C2 c2_0;


        int a;
        int sum;

        Cycle_B_2_00240_B1() {
            a3_0 = null;
            b2_0 = null;
            c2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a + b2_0.a + c2_0.a;
        }
    }

    class Cycle_B_2_00240_B2 {
        Cycle_B_2_00240_B1 b1_0;
        int a;
        int sum;

        Cycle_B_2_00240_B2() {
            b1_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_B_2_00240_C1 {
        Nocycle_B_2_00240_C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_B_2_00240_C1(String strObjectName) {
            c2_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        //   protected void finalize() throws java.lang.Throwable {
//       System.out.println("RC-Testing_Destruction_A1_"+strObjectName);
//   }
        void add() {
            sum = a + c2_0.a;
        }
    }

    class Nocycle_B_2_00240_C2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_B_2_00240_C2(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        //   protected void finalize() throws java.lang.Throwable {
//       System.out.println("RC-Testing_Destruction_B1_"+strObjectName);
//   }
        void add() {
            sum = a + a;
        }
    }
}


public class Cycle_Bm_2_10240 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Cycle_Bm_2_10240 A1_Cycle_Bm_2_10240 = new ThreadRc_Cycle_Bm_2_10240();
        ThreadRc_Cycle_Bm_2_10240 A2_Cycle_Bm_2_10240 = new ThreadRc_Cycle_Bm_2_10240();
        ThreadRc_Cycle_Bm_2_10240 A3_Cycle_Bm_2_10240 = new ThreadRc_Cycle_Bm_2_10240();
        ThreadRc_Cycle_Bm_2_10240 A4_Cycle_Bm_2_10240 = new ThreadRc_Cycle_Bm_2_10240();
        ThreadRc_Cycle_Bm_2_10240 A5_Cycle_Bm_2_10240 = new ThreadRc_Cycle_Bm_2_10240();
        ThreadRc_Cycle_Bm_2_10240 A6_Cycle_Bm_2_10240 = new ThreadRc_Cycle_Bm_2_10240();

        A1_Cycle_Bm_2_10240.start();
        A2_Cycle_Bm_2_10240.start();
        A3_Cycle_Bm_2_10240.start();
        A4_Cycle_Bm_2_10240.start();
        A5_Cycle_Bm_2_10240.start();
        A6_Cycle_Bm_2_10240.start();

        try {
            A1_Cycle_Bm_2_10240.join();
            A2_Cycle_Bm_2_10240.join();
            A3_Cycle_Bm_2_10240.join();
            A4_Cycle_Bm_2_10240.join();
            A5_Cycle_Bm_2_10240.join();
            A6_Cycle_Bm_2_10240.join();

        } catch (InterruptedException e) {
        }
        if (A1_Cycle_Bm_2_10240.check() && A2_Cycle_Bm_2_10240.check() && A3_Cycle_Bm_2_10240.check() && A4_Cycle_Bm_2_10240.check() && A5_Cycle_Bm_2_10240.check() && A6_Cycle_Bm_2_10240.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\nExpectResult\nExpectResult\n