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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Nocycle_am_2_00010.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Nocycle_a_2_00010 in RC测试-No-Cycle-00.vsd to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_am_2_00010.java
 *- @ExecuteClass: Nocycle_am_2_00010
 *- @ExecuteArgs:
 */
class ThreadRc_Nocycle_am_2_00010 extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_2_00010_A1 a1_main = new Nocycle_a_2_00010_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_2_00010_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_2_00010_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_2_00010_B3("b3_0");
        Nocycle_a_2_00010_A2 a2_main = new Nocycle_a_2_00010_A2("a2_main");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a2_main.add();
        //         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum);
        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a2_main.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1939)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_2_00010_A1 {
        Nocycle_a_2_00010_B1 b1_0;
        Nocycle_a_2_00010_B2 b2_0;
        Nocycle_a_2_00010_B3 b3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00010_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            b3_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a + b3_0.a;
        }
    }

    class Nocycle_a_2_00010_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00010_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00010_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00010_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00010_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00010_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00010_A2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00010_A2(String strObjectName) {
            a = 10;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}


public class Nocycle_am_2_00010 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Nocycle_am_2_00010 A1_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010();
        ThreadRc_Nocycle_am_2_00010 A2_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010();
        ThreadRc_Nocycle_am_2_00010 A3_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010();
        ThreadRc_Nocycle_am_2_00010 A4_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010();
        ThreadRc_Nocycle_am_2_00010 A5_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010();
        ThreadRc_Nocycle_am_2_00010 A6_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010();

        A1_Nocycle_am_2_00010.start();
        A2_Nocycle_am_2_00010.start();
        A3_Nocycle_am_2_00010.start();
        A4_Nocycle_am_2_00010.start();
        A5_Nocycle_am_2_00010.start();
        A6_Nocycle_am_2_00010.start();

        try {
            A1_Nocycle_am_2_00010.join();
            A2_Nocycle_am_2_00010.join();
            A3_Nocycle_am_2_00010.join();
            A4_Nocycle_am_2_00010.join();
            A5_Nocycle_am_2_00010.join();
            A6_Nocycle_am_2_00010.join();

        } catch (InterruptedException e) {
        }
        if (A1_Nocycle_am_2_00010.check() && A2_Nocycle_am_2_00010.check() && A3_Nocycle_am_2_00010.check() && A4_Nocycle_am_2_00010.check() && A5_Nocycle_am_2_00010.check() && A6_Nocycle_am_2_00010.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n