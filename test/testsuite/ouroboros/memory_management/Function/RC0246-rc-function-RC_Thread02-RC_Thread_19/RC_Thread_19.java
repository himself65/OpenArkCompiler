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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread02/RC_Thread_19
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Multi Thread reads or writes static para.mofidfy from Nocycle_a_00050
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_Thread_19.java
 *- @ExecuteClass: RC_Thread_19
 *- @ExecuteArgs:

 *
 */
class RC_Thread_19_1 extends Thread {
    public void run() {
        RC_Thread_19 rcth01 = new RC_Thread_19("rcth01");
        try {
            rcth01.addSum();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_19_2 extends Thread {
    public void run() {
        RC_Thread_19 rcth01 = new RC_Thread_19("rcth01");
        try {
            rcth01.ModifyB4();
        } catch (NullPointerException e) {

        }
    }
}

class RC_Thread_19_3 extends Thread {
    public void run() {
        RC_Thread_19 rcth01 = new RC_Thread_19("rcth01");
        try {
            rcth01.deleteA1();
        } catch (NullPointerException e) {

        }

    }
}

class RC_Thread_19_4 extends Thread {
    public void run() {
        RC_Thread_19 rcth01 = new RC_Thread_19("rcth01");
        try {
            rcth01.setA1(null);
        } catch (NullPointerException e) {

        }
    }
}


public class RC_Thread_19 {
    private static RC_Thread_19_A1 a1 = null;
    private volatile static RC_Thread_19_B4 b4;

    RC_Thread_19(String str) {
        try {
            a1 = new RC_Thread_19_A1("a1");
            a1.b1_0 = new RC_Thread_19_B1("b1_0");
            a1.b1_0.c1_0 = new RC_Thread_19_C1("c1_0");
            a1.b1_0.c2_0 = new RC_Thread_19_C2("c2_0");
            a1.b1_0.c3_0 = new RC_Thread_19_C3("c3_0");
            a1.b2_0 = new RC_Thread_19_B2("b2_0");
            a1.b3_0 = new RC_Thread_19_B3("b3_0");
            b4 = new RC_Thread_19_B4("b4_0");
            a1.b4_0 = b4;
        } catch (NullPointerException e) {

        }
    }

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
        System.out.println("ExpectResult");
    }

    private static void rc_testcase_main_wrapper() {
        RC_Thread_19_1 t1 = new RC_Thread_19_1();
        RC_Thread_19_2 t2 = new RC_Thread_19_2();
        RC_Thread_19_3 t3 = new RC_Thread_19_3();
        RC_Thread_19_4 t4 = new RC_Thread_19_4();
        t1.start();
        t2.start();
        t3.start();
        t4.start();
        try {
            t1.join();
            t2.join();
            t3.join();
            t4.join();

        } catch (InterruptedException e) {
        }
    }

    public void deleteA1() {
        a1 = null;
    }

    public void ModifyB4() {
        try {
            a1.b4_0 = new RC_Thread_19_B4("new_b4");
        } catch (NullPointerException e) {

        }
    }

    public void addSum() {
        try {
            a1.add();
            a1.b1_0.add();
            a1.b2_0.add();
            a1.b3_0.add();
            a1.b4_0.add();
            a1.b1_0.c1_0.add();
            a1.b1_0.c2_0.add();
            a1.b1_0.c3_0.add();
        } catch (NullPointerException e) {

        }
    }

    public void setA1(RC_Thread_19_A1 a1) {
        this.a1 = a1;
    }

    class RC_Thread_19_A1 {
        RC_Thread_19_B1 b1_0;
        RC_Thread_19_B2 b2_0;
        RC_Thread_19_B3 b3_0;
        RC_Thread_19_B4 b4_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_A1(String strObjectName) {
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

    class RC_Thread_19_B1 {
        RC_Thread_19_C1 c1_0;
        RC_Thread_19_C2 c2_0;
        RC_Thread_19_C3 c3_0;
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_B1(String strObjectName) {
            c1_0 = null;
            c2_0 = null;
            c3_0 = null;
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a + c2_0.a + c3_0.a;
        }
    }

    class RC_Thread_19_B2 {
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }


    class RC_Thread_19_B3 {
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class RC_Thread_19_B4 {
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_B4(String strObjectName) {
            a = 204;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class RC_Thread_19_C1 {
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_C1(String strObjectName) {
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class RC_Thread_19_C2 {
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_C2(String strObjectName) {
            a = 302;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class RC_Thread_19_C3 {
        int a;
        int sum;
        String strObjectName;

        RC_Thread_19_C3(String strObjectName) {
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


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n