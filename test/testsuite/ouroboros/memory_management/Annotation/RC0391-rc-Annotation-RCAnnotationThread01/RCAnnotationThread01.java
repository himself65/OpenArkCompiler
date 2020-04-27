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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread02/RCAnnotationThread01.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Multi Thread reads or writes static para.mofidfy from Nocycle_a_00180
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RCAnnotationThread01.java
 *- @ExecuteClass: RCAnnotationThread01
 *- @ExecuteArgs:

 *
 */

import com.huawei.ark.annotation.Unowned;

class RCAnnotationThread01_1 extends Thread {
    public void run() {
        RCAnnotationThread01 rcth01 = new RCAnnotationThread01();
        try {
            rcth01.setA1null();
        } catch (NullPointerException e) {

        }
    }
}

class RCAnnotationThread01_2 extends Thread {
    public void run() {
        RCAnnotationThread01 rcth01 = new RCAnnotationThread01();
        try {
            rcth01.setA2null();
        } catch (NullPointerException e) {

        }
    }
}

class RCAnnotationThread01_3 extends Thread {
    public void run() {
        RCAnnotationThread01 rcth01 = new RCAnnotationThread01();
        try {
            rcth01.setA3();
        } catch (NullPointerException e) {

        }

    }
}

class RCAnnotationThread01_4 extends Thread {
    public void run() {
        RCAnnotationThread01 rcth01 = new RCAnnotationThread01();
        try {
            rcth01.setA3null();
        } catch (NullPointerException e) {

        }
    }
}

class RCAnnotationThread01_5 extends Thread {
    public void run() {
        RCAnnotationThread01 rcth01 = new RCAnnotationThread01();
        try {
            rcth01.setA4null();
        } catch (NullPointerException e) {

        }
    }
}


public class RCAnnotationThread01 {
    private volatile static RCAnnotationThread01_A1 a1_main = null;
    private volatile static RCAnnotationThread01_A2 a2_main = null;
    private volatile static RCAnnotationThread01_A3 a3_main = null;
    private volatile static RCAnnotationThread01_A4 a4_main = null;

    RCAnnotationThread01() {
        synchronized (this) {
            try {
                a1_main = new RCAnnotationThread01_A1("a1_main");
                a2_main = new RCAnnotationThread01_A2("a2_main");
                a3_main = new RCAnnotationThread01_A3("a3_main");
                a4_main = new RCAnnotationThread01_A4("a4_main");
                a1_main.b1_0 = new RCAnnotationThread01_B1("b1_0");
                a1_main.d1_0 = new RCAnnotationThread01_D1("d1_0");
                a1_main.b1_0.d2_0 = new RCAnnotationThread01_D2("d2_0");

                a2_main.b2_0 = new RCAnnotationThread01_B2("b2_0");
                a2_main.b2_0.c1_0 = new RCAnnotationThread01_C1("c1_0");
                a2_main.b2_0.d1_0 = a1_main.d1_0;
                a2_main.b2_0.d2_0 = a1_main.b1_0.d2_0;
                a2_main.b2_0.d3_0 = new RCAnnotationThread01_D3("d3_0");
                a2_main.b2_0.c1_0.d1_0 = a1_main.d1_0;

                a3_main.b2_0 = a2_main.b2_0;
                a3_main.b2_0.c1_0 = a2_main.b2_0.c1_0;
                a3_main.b2_0.c1_0.d1_0 = a1_main.d1_0;
                a3_main.b2_0.d1_0 = a1_main.d1_0;
                a3_main.b2_0.d2_0 = a1_main.b1_0.d2_0;
                a3_main.b2_0.d3_0 = a2_main.b2_0.d3_0;

                a3_main.c2_0 = new RCAnnotationThread01_C2("c2_0");
                a3_main.c2_0.d2_0 = a1_main.b1_0.d2_0;
                a3_main.c2_0.d3_0 = a2_main.b2_0.d3_0;

                a4_main.b3_0 = new RCAnnotationThread01_B3("b3_0");
                a4_main.b3_0.c1_0 = a2_main.b2_0.c1_0;
                a4_main.b3_0.c1_0.d1_0 = a1_main.d1_0;
                a4_main.c2_0 = a3_main.c2_0;
                a4_main.c2_0.d2_0 = a1_main.b1_0.d2_0;
                a4_main.c2_0.d3_0 = a2_main.b2_0.d3_0;
            } catch (NullPointerException e) {

            }
        }
    }

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        System.out.println("ExpectResult");

    }

    private static void rc_testcase_main_wrapper() {
        RCAnnotationThread01_1 t1 = new RCAnnotationThread01_1();
        RCAnnotationThread01_2 t2 = new RCAnnotationThread01_2();
        RCAnnotationThread01_3 t3 = new RCAnnotationThread01_3();
        RCAnnotationThread01_4 t4 = new RCAnnotationThread01_4();
        RCAnnotationThread01_5 t5 = new RCAnnotationThread01_5();
        t1.start();
        t2.start();
        t3.start();
        t4.start();
        t5.start();
        try {
            t1.join();
            t2.join();
            t3.join();
            t4.join();
            t5.join();
        } catch (InterruptedException e) {
        }

    }


    public void setA1null() {
        a1_main = null;
    }

    public void setA2null() {
        a2_main = null;
    }

    public void setA3() {
        try {
            a3_main.c2_0.d2_0 = new RCAnnotationThread01_D2("new");
            a3_main = new RCAnnotationThread01_A3("a3_new");
            a3_main = null;
        } catch (NullPointerException e) {

        }
    }

    public void setA3null() {
        a3_main = null;
    }

    public void setA4null() {
        a4_main = null;
    }

    class RCAnnotationThread01_A1 {
        volatile RCAnnotationThread01_B1 b1_0;
        volatile RCAnnotationThread01_D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_A1(String strObjectName) {
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


    class RCAnnotationThread01_A2 {
        volatile RCAnnotationThread01_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_A2(String strObjectName) {
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


    class RCAnnotationThread01_A3 {
        @Unowned
        volatile RCAnnotationThread01_B2 b2_0;
        volatile RCAnnotationThread01_C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_A3(String strObjectName) {
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

    class RCAnnotationThread01_A4 {
        volatile RCAnnotationThread01_B3 b3_0;
        @Unowned
        volatile RCAnnotationThread01_C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_A4(String strObjectName) {
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


    class RCAnnotationThread01_B1 {
        volatile RCAnnotationThread01_D2 d2_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_B1(String strObjectName) {
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


    class RCAnnotationThread01_B2 {
        volatile RCAnnotationThread01_C1 c1_0;
        @Unowned
        volatile RCAnnotationThread01_D1 d1_0;
        @Unowned
        volatile RCAnnotationThread01_D2 d2_0;
        volatile RCAnnotationThread01_D3 d3_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_B2(String strObjectName) {
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


    class RCAnnotationThread01_B3 {
        @Unowned
        volatile RCAnnotationThread01_C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_B3(String strObjectName) {
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


    class RCAnnotationThread01_C1 {
        @Unowned
        volatile RCAnnotationThread01_D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_C1(String strObjectName) {
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

    class RCAnnotationThread01_C2 {
        @Unowned
        volatile RCAnnotationThread01_D2 d2_0;
        @Unowned
        volatile RCAnnotationThread01_D3 d3_0;
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_C2(String strObjectName) {
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


    class RCAnnotationThread01_D1 {
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_D1(String strObjectName) {
            a = 401;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class RCAnnotationThread01_D2 {
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_D2(String strObjectName) {
            a = 402;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class RCAnnotationThread01_D3 {
        int a;
        int sum;
        String strObjectName;

        RCAnnotationThread01_D3(String strObjectName) {
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


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n