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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Cycle_am_00510.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:make all Cycle_a_xx together to this Multi thread testcase for Performance Testing.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\nExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_am_00510.java
 *- @ExecuteClass: Cycle_am_00510
 *- @ExecuteArgs:
 */
class ThreadRc_Cycle_am_00180B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00180_A1 a1_main = new Cycle_a_00180_A1("a1_main");
        a1_main.a2_0 = new Cycle_a_00180_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00180_A3("a3_0");
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00180_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a5_0 = new Cycle_a_00180_A5("a5_0");
        a1_main.a2_0.a3_0.a4_0.a5_0.a6_0 = new Cycle_a_00180_A6("a6_0");
        a1_main.a2_0.a3_0.a4_0.a5_0.a6_0.a1_0 = a1_main;
        a1_main.a8_0 = new Cycle_a_00180_A8("a8_0");
        a1_main.a8_0.a9_0 = new Cycle_a_00180_A9("a9_0");
        a1_main.a8_0.a9_0.a7_0 = new Cycle_a_00180_A7("a7_0");
        a1_main.a8_0.a9_0.a7_0.a4_0 = a1_main.a2_0.a3_0.a4_0;

        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a2_0.a3_0.a4_0.a5_0.add();
        a1_main.a2_0.a3_0.a4_0.a5_0.a6_0.add();
        a1_main.a8_0.add();
        a1_main.a8_0.a9_0.add();
        a1_main.a8_0.a9_0.a7_0.add();


        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a2_0.a3_0.a4_0.a5_0.sum + a1_main.a2_0.a3_0.a4_0.a5_0.a6_0.sum + a1_main.a8_0.sum + a1_main.a8_0.a9_0.sum + a1_main.a8_0.a9_0.a7_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1994)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00180_A1 {
        Cycle_a_00180_A2 a2_0;
        Cycle_a_00180_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A1(String strObjectName) {
            a2_0 = null;
            a8_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a8_0.a;
        }
    }

    class Cycle_a_00180_A2 {
        Cycle_a_00180_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A2(String strObjectName) {
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

    class Cycle_a_00180_A3 {
        Cycle_a_00180_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A3(String strObjectName) {
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00180_A4 {
        Cycle_a_00180_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A4(String strObjectName) {
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

    class Cycle_a_00180_A5 {
        Cycle_a_00180_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A5(String strObjectName) {
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

    class Cycle_a_00180_A6 {
        Cycle_a_00180_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A6(String strObjectName) {
            a1_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_a_00180_A7 {
        Cycle_a_00180_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A7(String strObjectName) {
            a4_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00180_A8 {
        Cycle_a_00180_A9 a9_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A8(String strObjectName) {
            a9_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a9_0.a;
        }
    }

    class Cycle_a_00180_A9 {
        Cycle_a_00180_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00180_A9(String strObjectName) {
            a7_0 = null;
            a = 109;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00190B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00190_A1 a1_main = new Cycle_a_00190_A1("a1_main");
        Cycle_a_00190_A10 a10_main = new Cycle_a_00190_A10("a10_main");
        a1_main.a2_0 = new Cycle_a_00190_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00190_A3("a3_0");
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00190_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a5_0 = new Cycle_a_00190_A5("a5_0");
        a1_main.a2_0.a3_0.a4_0.a5_0.a6_0 = new Cycle_a_00190_A6("a6_0");
        a1_main.a2_0.a3_0.a4_0.a5_0.a6_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a4_0.a5_0.a6_0.a10_0 = a10_main;

        a10_main.a8_0 = new Cycle_a_00190_A8("a8_0");
        a10_main.a8_0.a9_0 = new Cycle_a_00190_A9("a9_0");
        a10_main.a8_0.a9_0.a7_0 = new Cycle_a_00190_A7("a7_0");
        a10_main.a8_0.a9_0.a7_0.a5_0 = a1_main.a2_0.a3_0.a4_0.a5_0;

        a1_main.add();
        a10_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a2_0.a3_0.a4_0.a5_0.add();
        a1_main.a2_0.a3_0.a4_0.a5_0.a6_0.add();
        a10_main.a8_0.add();
        a10_main.a8_0.a9_0.add();
        a10_main.a8_0.a9_0.a7_0.add();


        int result = a1_main.sum + a10_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a2_0.a3_0.a4_0.a5_0.sum + a1_main.a2_0.a3_0.a4_0.a5_0.a6_0.sum + a10_main.a8_0.sum + a10_main.a8_0.a9_0.sum + a10_main.a8_0.a9_0.a7_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2215)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00190_A1 {
        Cycle_a_00190_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A1(String strObjectName) {
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

    class Cycle_a_00190_A2 {
        Cycle_a_00190_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A2(String strObjectName) {
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

    class Cycle_a_00190_A3 {
        Cycle_a_00190_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A3(String strObjectName) {
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00190_A4 {
        Cycle_a_00190_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A4(String strObjectName) {
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

    class Cycle_a_00190_A5 {
        Cycle_a_00190_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A5(String strObjectName) {
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

    class Cycle_a_00190_A6 {
        Cycle_a_00190_A1 a1_0;
        Cycle_a_00190_A10 a10_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A6(String strObjectName) {
            a1_0 = null;
            a10_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a10_0.a;
        }
    }

    class Cycle_a_00190_A7 {
        Cycle_a_00190_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A7(String strObjectName) {
            a5_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a5_0.a;
        }
    }

    class Cycle_a_00190_A8 {
        Cycle_a_00190_A9 a9_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A8(String strObjectName) {
            a9_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a9_0.a;
        }
    }

    class Cycle_a_00190_A9 {
        Cycle_a_00190_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A9(String strObjectName) {
            a7_0 = null;
            a = 109;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00190_A10 {
        Cycle_a_00190_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00190_A10(String strObjectName) {
            a8_0 = null;
            a = 110;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A10_"+strObjectName);
        }

        void add() {
            sum = a + a8_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00200B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00200_A1 a1_main = new Cycle_a_00200_A1("a1_main");
        Cycle_a_00200_A4 a4_main = new Cycle_a_00200_A4("a4_main");
        a1_main.a2_0 = new Cycle_a_00200_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00200_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00200_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a4_0 = a4_main;
        a4_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a6_0 = new Cycle_a_00200_A6("a6_0");
        a1_main.a2_0.a6_0.a7_0 = new Cycle_a_00200_A7("a7_0");
        a1_main.a2_0.a7_0 = a1_main.a2_0.a6_0.a7_0;

        a1_main.add();
        a4_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a6_0.add();
        a1_main.a2_0.a6_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a1_main.a2_0.a6_0.sum + a1_main.a2_0.a6_0.a7_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1773)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00200_A1 {
        Cycle_a_00200_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00200_A1(String strObjectName) {
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

    class Cycle_a_00200_A2 {
        Cycle_a_00200_A3 a3_0;
        Cycle_a_00200_A6 a6_0;
        Cycle_a_00200_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00200_A2(String strObjectName) {
            a3_0 = null;
            a6_0 = null;
            a7_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a6_0.a + a7_0.a;
        }
    }

    class Cycle_a_00200_A3 {
        Cycle_a_00200_A1 a1_0;
        Cycle_a_00200_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00200_A3(String strObjectName) {
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

    class Cycle_a_00200_A4 {
        Cycle_a_00200_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00200_A4(String strObjectName) {
            a3_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00200_A5 {
        Cycle_a_00200_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00200_A5(String strObjectName) {
            a4_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00200_A6 {
        Cycle_a_00200_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00200_A6(String strObjectName) {
            a7_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00200_A7 {
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00200_A7(String strObjectName) {
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_Cycle_am_00210B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00210_A1 a1_main = new Cycle_a_00210_A1("a1_main");
        Cycle_a_00210_A4 a4_main = new Cycle_a_00210_A4("a4_main");
        a1_main.a2_0 = new Cycle_a_00210_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00210_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00210_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a4_0 = a4_main;
        a4_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a6_0 = new Cycle_a_00210_A6("a6_0");
        a1_main.a2_0.a3_0.a6_0.a7_0 = new Cycle_a_00210_A7("a7_0");
        a1_main.a2_0.a3_0.a6_0.a7_0.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a4_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a6_0.add();
        a1_main.a2_0.a3_0.a6_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a1_main.a2_0.a3_0.a6_0.sum + a1_main.a2_0.a3_0.a6_0.a7_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1662)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00210_A1 {
        Cycle_a_00210_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00210_A1(String strObjectName) {
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

    class Cycle_a_00210_A2 {
        Cycle_a_00210_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00210_A2(String strObjectName) {
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

    class Cycle_a_00210_A3 {
        Cycle_a_00210_A1 a1_0;
        Cycle_a_00210_A5 a5_0;
        Cycle_a_00210_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00210_A3(String strObjectName) {
            a1_0 = null;
            a5_0 = null;
            a6_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a5_0.a + a6_0.a;
        }
    }

    class Cycle_a_00210_A4 {
        Cycle_a_00210_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00210_A4(String strObjectName) {
            a3_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00210_A5 {
        Cycle_a_00210_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00210_A5(String strObjectName) {
            a4_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00210_A6 {
        Cycle_a_00210_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00210_A6(String strObjectName) {
            a7_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00210_A7 {
        Cycle_a_00210_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00210_A7(String strObjectName) {
            a3_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00220B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00220_A1 a1_main = new Cycle_a_00220_A1("a1_main");
        Cycle_a_00220_A4 a4_main = new Cycle_a_00220_A4("a4_main");
        Cycle_a_00220_A6 a6_main = new Cycle_a_00220_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00220_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00220_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00220_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a4_0 = a4_main;
        a4_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a5_0.a7_0 = new Cycle_a_00220_A7("a7_0");
        a6_main.a7_0 = a1_main.a2_0.a3_0.a5_0.a7_0;
        a6_main.a5_0 = a1_main.a2_0.a3_0.a5_0;

        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a5_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a6_main.a7_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1772)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00220_A1 {
        Cycle_a_00220_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00220_A1(String strObjectName) {
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

    class Cycle_a_00220_A2 {
        Cycle_a_00220_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00220_A2(String strObjectName) {
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

    class Cycle_a_00220_A3 {
        Cycle_a_00220_A1 a1_0;
        Cycle_a_00220_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00220_A3(String strObjectName) {
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

    class Cycle_a_00220_A4 {
        Cycle_a_00220_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00220_A4(String strObjectName) {
            a3_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00220_A5 {
        Cycle_a_00220_A4 a4_0;
        Cycle_a_00220_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00220_A5(String strObjectName) {
            a4_0 = null;
            a7_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a + a7_0.a;
        }
    }

    class Cycle_a_00220_A6 {
        Cycle_a_00220_A5 a5_0;
        Cycle_a_00220_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00220_A6(String strObjectName) {
            a5_0 = null;
            a7_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a5_0.a + a7_0.a;
        }
    }

    class Cycle_a_00220_A7 {
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00220_A7(String strObjectName) {
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_Cycle_am_00230B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00230_A1 a1_main = new Cycle_a_00230_A1("a1_main");
        Cycle_a_00230_A5 a5_main = new Cycle_a_00230_A5("a5_main");
        a1_main.a2_0 = new Cycle_a_00230_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00230_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00230_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a5_0 = a5_main;
        a5_main.a3_0 = a1_main.a2_0.a3_0;
        a5_main.a7_0 = new Cycle_a_00230_A7("a7_0");
        a5_main.a6_0 = new Cycle_a_00230_A6("a6_0");
        a5_main.a6_0.a7_0 = a5_main.a7_0;

        a1_main.add();
        a5_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a5_main.a7_0.add();
        a5_main.a6_0.add();

        int result = a1_main.sum + a5_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a5_main.a6_0.sum + a5_main.a7_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1773)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00230_A1 {
        Cycle_a_00230_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00230_A1(String strObjectName) {
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

    class Cycle_a_00230_A2 {
        Cycle_a_00230_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00230_A2(String strObjectName) {
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

    class Cycle_a_00230_A3 {
        Cycle_a_00230_A1 a1_0;
        Cycle_a_00230_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00230_A3(String strObjectName) {
            a1_0 = null;
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a4_0.a;
        }
    }

    class Cycle_a_00230_A4 {
        Cycle_a_00230_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00230_A4(String strObjectName) {
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

    class Cycle_a_00230_A5 {
        Cycle_a_00230_A3 a3_0;
        Cycle_a_00230_A6 a6_0;
        Cycle_a_00230_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00230_A5(String strObjectName) {
            a3_0 = null;
            a6_0 = null;
            a7_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a6_0.a + a7_0.a;
        }
    }

    class Cycle_a_00230_A6 {
        Cycle_a_00230_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00230_A6(String strObjectName) {
            a7_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00230_A7 {
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00230_A7(String strObjectName) {
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_Cycle_am_00240B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00240_A1 a1_main = new Cycle_a_00240_A1("a1_main");
        Cycle_a_00240_A5 a5_main = new Cycle_a_00240_A5("a5_main");
        a1_main.a2_0 = new Cycle_a_00240_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00240_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00240_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a5_0 = a5_main;
        a1_main.a6_0 = new Cycle_a_00240_A6("a6_0");
        a1_main.a6_0.a7_0 = new Cycle_a_00240_A7("a7_0");
        a1_main.a6_0.a7_0.a1_0 = a1_main;
        a5_main.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a5_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a6_0.add();
        a1_main.a6_0.a7_0.add();

        int result = a1_main.sum + a5_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a6_0.sum + a1_main.a6_0.a7_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1660)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00240_A1 {
        Cycle_a_00240_A2 a2_0;
        Cycle_a_00240_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00240_A1(String strObjectName) {
            a2_0 = null;
            a6_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a6_0.a;
        }
    }

    class Cycle_a_00240_A2 {
        Cycle_a_00240_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00240_A2(String strObjectName) {
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

    class Cycle_a_00240_A3 {
        Cycle_a_00240_A1 a1_0;
        Cycle_a_00240_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00240_A3(String strObjectName) {
            a1_0 = null;
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a4_0.a;
        }
    }

    class Cycle_a_00240_A4 {
        Cycle_a_00240_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00240_A4(String strObjectName) {
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

    class Cycle_a_00240_A5 {
        Cycle_a_00240_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00240_A5(String strObjectName) {
            a3_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00240_A6 {
        Cycle_a_00240_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00240_A6(String strObjectName) {
            a7_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00240_A7 {
        Cycle_a_00240_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00240_A7(String strObjectName) {
            a1_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00250B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00250_A1 a1_main = new Cycle_a_00250_A1("a1_main");
        Cycle_a_00250_A6 a6_main = new Cycle_a_00250_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00250_A2("a2_0");
        a1_main.a2_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0 = new Cycle_a_00250_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00250_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();

        int result = a1_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1238)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00250_A1 {
        Cycle_a_00250_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00250_A1(String strObjectName) {
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

    class Cycle_a_00250_A2 {
        Cycle_a_00250_A1 a1_0;
        Cycle_a_00250_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00250_A2(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00250_A3 {
        Cycle_a_00250_A1 a1_0;
        Cycle_a_00250_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00250_A3(String strObjectName) {
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

    class Cycle_a_00250_A5 {
        Cycle_a_00250_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00250_A5(String strObjectName) {
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

    class Cycle_a_00250_A6 {
        Cycle_a_00250_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00250_A6(String strObjectName) {
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00260B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00260_A1 a1_main = new Cycle_a_00260_A1("a1_main");
        Cycle_a_00260_A6 a6_main = new Cycle_a_00260_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00260_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00260_A3("a3_0");
        a1_main.a2_0.a3_0.a2_0 = a1_main.a2_0;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00260_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();

        int result = a1_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1239)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00260_A1 {
        Cycle_a_00260_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00260_A1(String strObjectName) {
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

    class Cycle_a_00260_A2 {
        Cycle_a_00260_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00260_A2(String strObjectName) {
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

    class Cycle_a_00260_A3 {
        Cycle_a_00260_A1 a1_0;
        Cycle_a_00260_A2 a2_0;
        Cycle_a_00260_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00260_A3(String strObjectName) {
            a1_0 = null;
            a2_0 = null;
            a5_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a2_0.a + a5_0.a;
        }
    }

    class Cycle_a_00260_A5 {
        Cycle_a_00260_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00260_A5(String strObjectName) {
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

    class Cycle_a_00260_A6 {
        Cycle_a_00260_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00260_A6(String strObjectName) {
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00270B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00270_A1 a1_main = new Cycle_a_00270_A1("a1_main");
        Cycle_a_00270_A6 a6_main = new Cycle_a_00270_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00270_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00270_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00270_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();

        int result = a1_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1240)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00270_A1 {
        Cycle_a_00270_A2 a2_0;
        Cycle_a_00270_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00270_A1(String strObjectName) {
            a2_0 = null;
            a3_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a3_0.a;
        }
    }

    class Cycle_a_00270_A2 {
        Cycle_a_00270_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00270_A2(String strObjectName) {
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

    class Cycle_a_00270_A3 {
        Cycle_a_00270_A1 a1_0;
        Cycle_a_00270_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00270_A3(String strObjectName) {
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

    class Cycle_a_00270_A5 {
        Cycle_a_00270_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00270_A5(String strObjectName) {
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

    class Cycle_a_00270_A6 {
        Cycle_a_00270_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00270_A6(String strObjectName) {
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00280B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00280_A1 a1_main = new Cycle_a_00280_A1("a1_main");
        Cycle_a_00280_A6 a6_main = new Cycle_a_00280_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00280_A2("a2_0");
        a1_main.a2_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0 = new Cycle_a_00280_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a2_0 = a1_main.a2_0;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00280_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a1_main.a3_0 = a1_main.a2_0.a3_0;
        a6_main.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();

        int result = a1_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1443)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00280_A1 {
        Cycle_a_00280_A2 a2_0;
        Cycle_a_00280_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00280_A1(String strObjectName) {
            a2_0 = null;
            a3_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a3_0.a;
        }
    }

    class Cycle_a_00280_A2 {
        Cycle_a_00280_A1 a1_0;
        Cycle_a_00280_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00280_A2(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00280_A3 {
        Cycle_a_00280_A1 a1_0;
        Cycle_a_00280_A2 a2_0;
        Cycle_a_00280_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00280_A3(String strObjectName) {
            a1_0 = null;
            a2_0 = null;
            a5_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a2_0.a + a5_0.a;
        }
    }

    class Cycle_a_00280_A5 {
        Cycle_a_00280_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00280_A5(String strObjectName) {
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

    class Cycle_a_00280_A6 {
        Cycle_a_00280_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00280_A6(String strObjectName) {
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00290B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00290_A1 a1_main = new Cycle_a_00290_A1("a1_main");
        Cycle_a_00290_A5 a5_main = new Cycle_a_00290_A5("a5_main");
        a1_main.a2_0 = new Cycle_a_00290_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00290_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00290_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a5_0 = a5_main;
        a1_main.a2_0.a3_0.a6_0 = new Cycle_a_00290_A6("a6_0");
        a1_main.a2_0.a3_0.a6_0.a2_0 = a1_main.a2_0;
        a5_main.a3_0 = a1_main.a2_0.a3_0;


        a1_main.add();
        a5_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a2_0.a3_0.a6_0.add();

        int result = a1_main.sum + a5_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a2_0.a3_0.a6_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1447)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00290_A1 {
        Cycle_a_00290_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00290_A1(String strObjectName) {
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

    class Cycle_a_00290_A2 {
        Cycle_a_00290_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00290_A2(String strObjectName) {
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

    class Cycle_a_00290_A3 {
        Cycle_a_00290_A1 a1_0;
        Cycle_a_00290_A4 a4_0;
        Cycle_a_00290_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00290_A3(String strObjectName) {
            a1_0 = null;
            a4_0 = null;
            a6_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a4_0.a + a6_0.a;
        }
    }

    class Cycle_a_00290_A4 {
        Cycle_a_00290_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00290_A4(String strObjectName) {
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

    class Cycle_a_00290_A5 {
        Cycle_a_00290_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00290_A5(String strObjectName) {
            a3_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00290_A6 {
        Cycle_a_00290_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00290_A6(String strObjectName) {
            a2_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00300B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00300_A1 a1_main = new Cycle_a_00300_A1("a1_main");
        Cycle_a_00300_A5 a5_main = new Cycle_a_00300_A5("a5_main");
        a1_main.a2_0 = new Cycle_a_00300_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00300_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00300_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a5_0 = a5_main;
        a5_main.a3_0 = a1_main.a2_0.a3_0;
        a5_main.a3_0.a4_0.a6_0 = new Cycle_a_00300_A6("a6_0");
        a5_main.a3_0.a4_0.a6_0.a3_0 = a1_main.a2_0.a3_0;
        a5_main.a3_0.a4_0.a5_0 = a5_main;

        a1_main.add();
        a5_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a2_0.a3_0.a4_0.a6_0.add();

        int result = a1_main.sum + a5_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a2_0.a3_0.a4_0.a6_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1448)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00300_A1 {
        Cycle_a_00300_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00300_A1(String strObjectName) {
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

    class Cycle_a_00300_A2 {
        Cycle_a_00300_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00300_A2(String strObjectName) {
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

    class Cycle_a_00300_A3 {
        Cycle_a_00300_A1 a1_0;
        Cycle_a_00300_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00300_A3(String strObjectName) {
            a1_0 = null;
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a4_0.a;
        }
    }

    class Cycle_a_00300_A4 {
        Cycle_a_00300_A5 a5_0;
        Cycle_a_00300_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00300_A4(String strObjectName) {
            a5_0 = null;
            a6_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a5_0.a + a6_0.a;
        }
    }

    class Cycle_a_00300_A5 {
        Cycle_a_00300_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00300_A5(String strObjectName) {
            a3_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00300_A6 {
        Cycle_a_00300_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00300_A6(String strObjectName) {
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00310B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00310_A1 a1_main = new Cycle_a_00310_A1("a1_main");
        Cycle_a_00310_A4 a4_main = new Cycle_a_00310_A4("a4_main");
        Cycle_a_00310_A6 a6_main = new Cycle_a_00310_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00310_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00310_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00310_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a1_main.a4_0 = a4_main;
        a6_main.a1_0 = a1_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;
        a6_main.a3_0.a5_0 = a1_main.a2_0.a3_0.a5_0;
        a4_main.a6_0 = a6_main;

        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1552)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00310_A1 {
        Cycle_a_00310_A2 a2_0;
        Cycle_a_00310_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00310_A1(String strObjectName) {
            a2_0 = null;
            a4_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a4_0.a;
        }
    }

    class Cycle_a_00310_A2 {
        Cycle_a_00310_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00310_A2(String strObjectName) {
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

    class Cycle_a_00310_A3 {
        Cycle_a_00310_A1 a1_0;
        Cycle_a_00310_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00310_A3(String strObjectName) {
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

    class Cycle_a_00310_A4 {
        Cycle_a_00310_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00310_A4(String strObjectName) {
            a6_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a;
        }
    }

    class Cycle_a_00310_A5 {
        Cycle_a_00310_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00310_A5(String strObjectName) {
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

    class Cycle_a_00310_A6 {
        Cycle_a_00310_A1 a1_0;
        Cycle_a_00310_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00310_A6(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00320B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00320_A1 a1_main = new Cycle_a_00320_A1("a1_main");
        Cycle_a_00320_A4 a4_main = new Cycle_a_00320_A4("a4_main");
        Cycle_a_00320_A6 a6_main = new Cycle_a_00320_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00320_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00320_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00320_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a1_0 = a1_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;
        a4_main.a5_0 = a1_main.a2_0.a3_0.a5_0;
        a6_main.a4_0 = a4_main;

        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1551)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00320_A1 {
        Cycle_a_00320_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A1(String strObjectName) {
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

    class Cycle_a_00320_A2 {
        Cycle_a_00320_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A2(String strObjectName) {
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

    class Cycle_a_00320_A3 {
        Cycle_a_00320_A1 a1_0;
        Cycle_a_00320_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A3(String strObjectName) {
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

    class Cycle_a_00320_A4 {
        Cycle_a_00320_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A4(String strObjectName) {
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

    class Cycle_a_00320_A5 {
        Cycle_a_00320_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A5(String strObjectName) {
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

    class Cycle_a_00320_A6 {
        Cycle_a_00320_A1 a1_0;
        Cycle_a_00320_A3 a3_0;
        Cycle_a_00320_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00320_A6(String strObjectName) {
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

class ThreadRc_Cycle_am_00330B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00330_A1 a1_main = new Cycle_a_00330_A1("a1_main");
        Cycle_a_00330_A4 a4_main = new Cycle_a_00330_A4("a4_main");
        Cycle_a_00330_A6 a6_main = new Cycle_a_00330_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00330_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00330_A3("a3_0");
        a1_main.a2_0.a4_0 = a4_main;
        a4_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00330_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a1_main.a2_0.a3_0.a5_0.a8_0 = new Cycle_a_00330_A8("a8_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0 = new Cycle_a_00330_A7("a7_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.a3_0 = a1_main.a2_0.a3_0;
        a6_main.a1_0 = a1_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;


        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a6_main.a3_0.a5_0.a8_0.a7_0.sum + a6_main.a3_0.a5_0.a8_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2080)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00330_A1 {
        Cycle_a_00330_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A1(String strObjectName) {
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

    class Cycle_a_00330_A2 {
        Cycle_a_00330_A3 a3_0;
        Cycle_a_00330_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A2(String strObjectName) {
            a3_0 = null;
            a4_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_a_00330_A3 {
        Cycle_a_00330_A1 a1_0;
        Cycle_a_00330_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A3(String strObjectName) {
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

    class Cycle_a_00330_A4 {
        Cycle_a_00330_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A4(String strObjectName) {
            a1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_a_00330_A5 {
        Cycle_a_00330_A6 a6_0;
        Cycle_a_00330_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A5(String strObjectName) {
            a6_0 = null;
            a8_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a + a8_0.a;
        }
    }

    class Cycle_a_00330_A6 {
        Cycle_a_00330_A1 a1_0;
        Cycle_a_00330_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A6(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00330_A7 {
        Cycle_a_00330_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A7(String strObjectName) {
            a3_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00330_A8 {
        Cycle_a_00330_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00330_A8(String strObjectName) {
            a7_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00340B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00340_A1 a1_main = new Cycle_a_00340_A1("a1_main");
        Cycle_a_00340_A4 a4_main = new Cycle_a_00340_A4("a4_main");
        Cycle_a_00340_A6 a6_main = new Cycle_a_00340_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00340_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00340_A3("a3_0");
        a1_main.a2_0.a4_0 = a4_main;
        a4_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00340_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a1_main.a2_0.a3_0.a8_0 = new Cycle_a_00340_A8("a8_0");
        a1_main.a2_0.a3_0.a8_0.a7_0 = new Cycle_a_00340_A7("a7_0");
        a1_main.a2_0.a3_0.a8_0.a7_0.a2_0 = a1_main.a2_0;
        a6_main.a1_0 = a1_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;


        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a8_0.add();
        a1_main.a2_0.a3_0.a8_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a6_main.a3_0.a8_0.a7_0.sum + a6_main.a3_0.a8_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2079)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00340_A1 {
        Cycle_a_00340_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A1(String strObjectName) {
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

    class Cycle_a_00340_A2 {
        Cycle_a_00340_A3 a3_0;
        Cycle_a_00340_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A2(String strObjectName) {
            a3_0 = null;
            a4_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_a_00340_A3 {
        Cycle_a_00340_A1 a1_0;
        Cycle_a_00340_A5 a5_0;
        Cycle_a_00340_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A3(String strObjectName) {
            a1_0 = null;
            a5_0 = null;
            a8_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a5_0.a + a8_0.a;
        }
    }

    class Cycle_a_00340_A4 {
        Cycle_a_00340_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A4(String strObjectName) {
            a1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_a_00340_A5 {
        Cycle_a_00340_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A5(String strObjectName) {
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

    class Cycle_a_00340_A6 {
        Cycle_a_00340_A1 a1_0;
        Cycle_a_00340_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A6(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00340_A7 {
        Cycle_a_00340_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A7(String strObjectName) {
            a2_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_a_00340_A8 {
        Cycle_a_00340_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00340_A8(String strObjectName) {
            a7_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00350B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00350_A1 a1_main = new Cycle_a_00350_A1("a1_main");
        Cycle_a_00350_A4 a4_main = new Cycle_a_00350_A4("a4_main");
        Cycle_a_00350_A6 a6_main = new Cycle_a_00350_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_00350_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00350_A3("a3_0");
        a1_main.a2_0.a4_0 = a4_main;
        a4_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00350_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0.a8_0 = new Cycle_a_00350_A8("a8_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0 = new Cycle_a_00350_A7("a7_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.a2_0 = a1_main.a2_0;
        a6_main.a3_0 = a1_main.a2_0.a3_0;


        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a6_main.a3_0.a5_0.a8_0.a7_0.sum + a6_main.a3_0.a5_0.a8_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2079)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00350_A1 {
        Cycle_a_00350_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A1(String strObjectName) {
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

    class Cycle_a_00350_A2 {
        Cycle_a_00350_A3 a3_0;
        Cycle_a_00350_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A2(String strObjectName) {
            a3_0 = null;
            a4_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_a_00350_A3 {
        Cycle_a_00350_A1 a1_0;
        Cycle_a_00350_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A3(String strObjectName) {
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

    class Cycle_a_00350_A4 {
        Cycle_a_00350_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A4(String strObjectName) {
            a1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_a_00350_A5 {
        Cycle_a_00350_A6 a6_0;
        Cycle_a_00350_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A5(String strObjectName) {
            a6_0 = null;
            a8_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a + a8_0.a;
        }
    }

    class Cycle_a_00350_A6 {
        Cycle_a_00350_A1 a1_0;
        Cycle_a_00350_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A6(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00350_A7 {
        Cycle_a_00350_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A7(String strObjectName) {
            a2_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_a_00350_A8 {
        Cycle_a_00350_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00350_A8(String strObjectName) {
            a7_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00360B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00360_A1 a1_main = new Cycle_a_00360_A1("a1_main");
        Cycle_a_00360_A4 a4_main = new Cycle_a_00360_A4("a4_main");
        Cycle_a_00360_A6 a6_main = new Cycle_a_00360_A6("a6_main");
        Cycle_a_00360_A9 a9_main = new Cycle_a_00360_A9("a9_main");
        a1_main.a2_0 = new Cycle_a_00360_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00360_A3("a3_0");
        a1_main.a2_0.a4_0 = a4_main;
        a4_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00360_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0.a8_0 = new Cycle_a_00360_A8("a8_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0 = new Cycle_a_00360_A7("a7_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.a2_0 = a1_main.a2_0;
        a1_main.a2_0.a3_0.a5_0.a9_0 = a9_main;
        a9_main.a2_0 = a1_main.a2_0;
        a6_main.a3_0 = a1_main.a2_0.a3_0;


        a1_main.add();
        a4_main.add();
        a6_main.add();
        a9_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a9_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a6_main.a3_0.a5_0.a8_0.a7_0.sum + a6_main.a3_0.a5_0.a8_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2399)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00360_A1 {
        Cycle_a_00360_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A1(String strObjectName) {
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

    class Cycle_a_00360_A2 {
        Cycle_a_00360_A3 a3_0;
        Cycle_a_00360_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A2(String strObjectName) {
            a3_0 = null;
            a4_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_a_00360_A3 {
        Cycle_a_00360_A1 a1_0;
        Cycle_a_00360_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A3(String strObjectName) {
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

    class Cycle_a_00360_A4 {
        Cycle_a_00360_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A4(String strObjectName) {
            a1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_a_00360_A5 {
        Cycle_a_00360_A6 a6_0;
        Cycle_a_00360_A8 a8_0;
        Cycle_a_00360_A9 a9_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A5(String strObjectName) {
            a6_0 = null;
            a8_0 = null;
            a9_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a + a8_0.a + a9_0.a;
        }
    }

    class Cycle_a_00360_A6 {
        Cycle_a_00360_A1 a1_0;
        Cycle_a_00360_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A6(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00360_A7 {
        Cycle_a_00360_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A7(String strObjectName) {
            a2_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_a_00360_A8 {
        Cycle_a_00360_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A8(String strObjectName) {
            a7_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00360_A9 {
        Cycle_a_00360_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00360_A9(String strObjectName) {
            a2_0 = null;
            a = 109;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A9_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00370B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00370_A1 a1_main = new Cycle_a_00370_A1("a1_main");
        Cycle_a_00370_A4 a4_main = new Cycle_a_00370_A4("a4_main");
        Cycle_a_00370_A6 a6_main = new Cycle_a_00370_A6("a6_main");
        Cycle_a_00370_A9 a9_main = new Cycle_a_00370_A9("a9_main");
        a1_main.a2_0 = new Cycle_a_00370_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00370_A3("a3_0");
        a1_main.a2_0.a4_0 = a4_main;
        a4_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00370_A5("a5_0");
        a1_main.a2_0.a3_0.a10_0 = new Cycle_a_00370_A10("a10_0");
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0.a8_0 = new Cycle_a_00370_A8("a8_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0 = new Cycle_a_00370_A7("a7_0");
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.a2_0 = a1_main.a2_0;
        a1_main.a2_0.a3_0.a10_0.a9_0 = a9_main;
        a9_main.a6_0 = a6_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;


        a1_main.add();
        a4_main.add();
        a6_main.add();
        a9_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a10_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.add();
        a1_main.a2_0.a3_0.a5_0.a8_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a6_main.sum + a9_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a6_main.a3_0.a5_0.a8_0.a7_0.sum + a6_main.a3_0.a5_0.a8_0.sum + a6_main.a3_0.a10_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2623)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00370_A1 {
        Cycle_a_00370_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A1(String strObjectName) {
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

    class Cycle_a_00370_A2 {
        Cycle_a_00370_A3 a3_0;
        Cycle_a_00370_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A2(String strObjectName) {
            a3_0 = null;
            a4_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_a_00370_A3 {
        Cycle_a_00370_A1 a1_0;
        Cycle_a_00370_A5 a5_0;
        Cycle_a_00370_A10 a10_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A3(String strObjectName) {
            a1_0 = null;
            a5_0 = null;
            a10_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a5_0.a + a10_0.a;
        }
    }

    class Cycle_a_00370_A4 {
        Cycle_a_00370_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A4(String strObjectName) {
            a1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_a_00370_A5 {
        Cycle_a_00370_A6 a6_0;
        Cycle_a_00370_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A5(String strObjectName) {
            a6_0 = null;
            a8_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a + a8_0.a;
        }
    }

    class Cycle_a_00370_A6 {
        Cycle_a_00370_A1 a1_0;
        Cycle_a_00370_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A6(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00370_A7 {
        Cycle_a_00370_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A7(String strObjectName) {
            a2_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_a_00370_A8 {
        Cycle_a_00370_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A8(String strObjectName) {
            a7_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00370_A9 {
        Cycle_a_00370_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A9(String strObjectName) {
            a6_0 = null;
            a = 109;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A9_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a;
        }
    }

    class Cycle_a_00370_A10 {
        Cycle_a_00370_A9 a9_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00370_A10(String strObjectName) {
            a9_0 = null;
            a = 110;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A10_"+strObjectName);
        }

        void add() {
            sum = a + a9_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00380B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00380_A1 a1_main = new Cycle_a_00380_A1("a1_main");
        a1_main.a2_0 = new Cycle_a_00380_A2("a2_0");
        a1_main.a2_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0 = new Cycle_a_00380_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a2_0 = a1_main.a2_0;


        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();

        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 815)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00380_A1 {
        Cycle_a_00380_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00380_A1(String strObjectName) {
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

    class Cycle_a_00380_A2 {
        Cycle_a_00380_A1 a1_0;
        Cycle_a_00380_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00380_A2(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00380_A3 {
        Cycle_a_00380_A1 a1_0;
        Cycle_a_00380_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00380_A3(String strObjectName) {
            a1_0 = null;
            a2_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a2_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00390B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00390_A1 a1_main = new Cycle_a_00390_A1("a1_main");
        a1_main.a2_0 = new Cycle_a_00390_A2("a2_0");
        a1_main.a2_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0 = new Cycle_a_00390_A3("a3_0");
        a1_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a3_0.a1_0 = a1_main;
        a1_main.a3_0.a2_0 = a1_main.a2_0;


        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();

        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 918)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00390_A1 {
        Cycle_a_00390_A2 a2_0;
        Cycle_a_00390_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00390_A1(String strObjectName) {
            a2_0 = null;
            a3_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a3_0.a;
        }
    }

    class Cycle_a_00390_A2 {
        Cycle_a_00390_A1 a1_0;
        Cycle_a_00390_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00390_A2(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00390_A3 {
        Cycle_a_00390_A1 a1_0;
        Cycle_a_00390_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00390_A3(String strObjectName) {
            a1_0 = null;
            a2_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a2_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00400B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00400_A1 a1_main = new Cycle_a_00400_A1("a1_main");
        a1_main.a2_0 = new Cycle_a_00400_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00400_A3("a3_0");
        a1_main.a4_0 = new Cycle_a_00400_A4("a4_0");
        a1_main.a4_0.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a1_0 = a1_main;

        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a4_0.add();

        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a4_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 923)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00400_A1 {
        Cycle_a_00400_A2 a2_0;
        Cycle_a_00400_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00400_A1(String strObjectName) {
            a2_0 = null;
            a4_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a4_0.a;
        }
    }

    class Cycle_a_00400_A2 {
        Cycle_a_00400_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00400_A2(String strObjectName) {
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

    class Cycle_a_00400_A3 {
        Cycle_a_00400_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00400_A3(String strObjectName) {
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

    class Cycle_a_00400_A4 {
        Cycle_a_00400_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00400_A4(String strObjectName) {
            a3_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00410B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00410_A1 a1_main = new Cycle_a_00410_A1("a1_main");
        a1_main.a2_0 = new Cycle_a_00410_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00410_A3("a3_0");
        a1_main.a2_0.a4_0 = new Cycle_a_00410_A4("a4_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a4_0.a1_0 = a1_main;

        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a4_0.add();

        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a4_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 921)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00410_A1 {
        Cycle_a_00410_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00410_A1(String strObjectName) {
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

    class Cycle_a_00410_A2 {
        Cycle_a_00410_A3 a3_0;
        Cycle_a_00410_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00410_A2(String strObjectName) {
            a3_0 = null;
            a4_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_a_00410_A3 {
        Cycle_a_00410_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00410_A3(String strObjectName) {
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

    class Cycle_a_00410_A4 {
        Cycle_a_00410_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00410_A4(String strObjectName) {
            a1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00420B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00420_A1 a1_main = new Cycle_a_00420_A1("a1_main");
//         Cycle_a_00420_A4 a4_main = new Cycle_a_00420_A4("a4_main");
        a1_main.a2_0 = new Cycle_a_00420_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00420_A3("a3_0");
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00420_A4("a4_0");
        a1_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a4_0.a1_0 = a1_main;

        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();

        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 923)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00420_A1 {
        Cycle_a_00420_A2 a2_0;
        Cycle_a_00420_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00420_A1(String strObjectName) {
            a2_0 = null;
            a3_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a3_0.a;
        }
    }

    class Cycle_a_00420_A2 {
        Cycle_a_00420_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00420_A2(String strObjectName) {
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

    class Cycle_a_00420_A3 {
        Cycle_a_00420_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00420_A3(String strObjectName) {
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00420_A4 {
        Cycle_a_00420_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00420_A4(String strObjectName) {
            a1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00430B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00430_A1 a1_main = new Cycle_a_00430_A1("a1_main");
        a1_main.a2_0 = new Cycle_a_00430_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00430_A3("a3_0");
        a1_main.a2_0.a4_0 = new Cycle_a_00430_A4("a4_0");
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

        if (result == 1124)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00430_A1 {
        Cycle_a_00430_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00430_A1(String strObjectName) {
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

    class Cycle_a_00430_A2 {
        Cycle_a_00430_A1 a1_0;
        Cycle_a_00430_A3 a3_0;
        Cycle_a_00430_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00430_A2(String strObjectName) {
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

    class Cycle_a_00430_A3 {
        Cycle_a_00430_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00430_A3(String strObjectName) {
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

    class Cycle_a_00430_A4 {
        Cycle_a_00430_A1 a1_0;
        Cycle_a_00430_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00430_A4(String strObjectName) {
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
}

class ThreadRc_Cycle_am_00440B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00440_A1 a1_main = new Cycle_a_00440_A1("a1_main");
        a1_main.a2_0 = new Cycle_a_00440_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00440_A3("a3_0");
        a1_main.a2_0.a4_0 = new Cycle_a_00440_A4("a4_0");
        a1_main.a2_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a4_0.a2_0 = a1_main.a2_0;


        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a4_0.add();


        int result = a1_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a4_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1023)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00440_A1 {
        Cycle_a_00440_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00440_A1(String strObjectName) {
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

    class Cycle_a_00440_A2 {
        Cycle_a_00440_A1 a1_0;
        Cycle_a_00440_A3 a3_0;
        Cycle_a_00440_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00440_A2(String strObjectName) {
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

    class Cycle_a_00440_A3 {
        Cycle_a_00440_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00440_A3(String strObjectName) {
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

    class Cycle_a_00440_A4 {
        Cycle_a_00440_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00440_A4(String strObjectName) {
            a2_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00450B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00450_A1 a1_main = new Cycle_a_00450_A1("a1_main");
        Cycle_a_00450_A6 a6_main = new Cycle_a_00450_A6("a6_main");
        Cycle_a_00450_A7 a7_main = new Cycle_a_00450_A7("a7_main");
        a1_main.a2_0 = new Cycle_a_00450_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00450_A3("a3_0");
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00450_A5("a5_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0.a6_0 = a6_main;
        a6_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a5_0.a7_0 = a7_main;
        a7_main.a2_0 = a1_main.a2_0;

        a1_main.add();
        a6_main.add();
        a7_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();


        int result = a1_main.sum + a6_main.sum + a7_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1453)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00450_A1 {
        Cycle_a_00450_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A1(String strObjectName) {
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

    class Cycle_a_00450_A2 {
        Cycle_a_00450_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A2(String strObjectName) {
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

    class Cycle_a_00450_A3 {
        Cycle_a_00450_A1 a1_0;
        Cycle_a_00450_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A3(String strObjectName) {
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

    class Cycle_a_00450_A5 {
        Cycle_a_00450_A6 a6_0;
        Cycle_a_00450_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A5(String strObjectName) {
            a6_0 = null;
            a7_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a + a7_0.a;
        }
    }

    class Cycle_a_00450_A6 {
        Cycle_a_00450_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A6(String strObjectName) {
            a3_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00450_A7 {
        Cycle_a_00450_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00450_A7(String strObjectName) {
            a2_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00460B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00460_A1 a1_main = new Cycle_a_00460_A1("a1_main");
        Cycle_a_00460_A5 a5_main = new Cycle_a_00460_A5("a5_main");
        a1_main.a2_0 = new Cycle_a_00460_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00460_A3("a3_0");
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00460_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a1_0 = a1_main;
        a1_main.a4_0 = a1_main.a2_0.a3_0.a4_0;
        a1_main.a2_0.a3_0.a4_0.a6_0 = new Cycle_a_00460_A6("a6_0");
        a1_main.a2_0.a3_0.a4_0.a6_0.a5_0 = a5_main;
        a1_main.a2_0.a3_0.a4_0.a6_0.a1_0 = a1_main;

        a5_main.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a5_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a2_0.a3_0.a4_0.a6_0.add();

        int result = a1_main.sum + a5_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a2_0.a3_0.a4_0.a6_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1550)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00460_A1 {
        Cycle_a_00460_A2 a2_0;
        Cycle_a_00460_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00460_A1(String strObjectName) {
            a2_0 = null;
            a4_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + a2_0.a + a4_0.a;
        }
    }

    class Cycle_a_00460_A2 {
        Cycle_a_00460_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00460_A2(String strObjectName) {
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

    class Cycle_a_00460_A3 {
        Cycle_a_00460_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00460_A3(String strObjectName) {
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00460_A4 {
        Cycle_a_00460_A1 a1_0;
        Cycle_a_00460_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00460_A4(String strObjectName) {
            a1_0 = null;
            a6_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a6_0.a;
        }
    }

    class Cycle_a_00460_A5 {
        Cycle_a_00460_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00460_A5(String strObjectName) {
            a3_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00460_A6 {
        Cycle_a_00460_A1 a1_0;
        Cycle_a_00460_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00460_A6(String strObjectName) {
            a1_0 = null;
            a5_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a5_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00470B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00470_A1 a1_main = new Cycle_a_00470_A1("a1_main");
        Cycle_a_00470_A5 a5_main = new Cycle_a_00470_A5("a5_main");
        a1_main.a2_0 = new Cycle_a_00470_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00470_A3("a3_0");
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00470_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a4_0.a6_0 = new Cycle_a_00470_A6("a6_0");
        a1_main.a2_0.a3_0.a4_0.a6_0.a5_0 = a5_main;

        a5_main.a3_0 = a1_main.a2_0.a3_0;
        a5_main.a1_0 = a1_main;

        a1_main.add();
        a5_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a2_0.a3_0.a4_0.a6_0.add();

        int result = a1_main.sum + a5_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a2_0.a3_0.a4_0.a6_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1446)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00470_A1 {
        Cycle_a_00470_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00470_A1(String strObjectName) {
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

    class Cycle_a_00470_A2 {
        Cycle_a_00470_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00470_A2(String strObjectName) {
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

    class Cycle_a_00470_A3 {
        Cycle_a_00470_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00470_A3(String strObjectName) {
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00470_A4 {
        Cycle_a_00470_A1 a1_0;
        Cycle_a_00470_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00470_A4(String strObjectName) {
            a1_0 = null;
            a6_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a6_0.a;
        }
    }

    class Cycle_a_00470_A5 {
        Cycle_a_00470_A1 a1_0;
        Cycle_a_00470_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00470_A5(String strObjectName) {
            a1_0 = null;
            a3_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a3_0.a;
        }
    }

    class Cycle_a_00470_A6 {
        Cycle_a_00470_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00470_A6(String strObjectName) {
            a5_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a5_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00480B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00480_A1 a1_main = new Cycle_a_00480_A1("a1_main");
        Cycle_a_00480_A5 a5_main = new Cycle_a_00480_A5("a5_main");
        a1_main.a2_0 = new Cycle_a_00480_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00480_A3("a3_0");
        a1_main.a2_0.a3_0.a4_0 = new Cycle_a_00480_A4("a4_0");
        a1_main.a2_0.a3_0.a4_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a4_0.a6_0 = new Cycle_a_00480_A6("a6_0");
        a1_main.a2_0.a3_0.a4_0.a6_0.a5_0 = a5_main;
        a1_main.a2_0.a3_0.a4_0.a6_0.a4_0 = a1_main.a2_0.a3_0.a4_0;
        a1_main.a2_0.a3_0.a4_0.a6_0.a1_0 = a1_main;

        a5_main.a3_0 = a1_main.a2_0.a3_0;
        a5_main.a3_0.a4_0 = a1_main.a2_0.a3_0.a4_0;


        a1_main.add();
        a5_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a4_0.add();
        a1_main.a2_0.a3_0.a4_0.a1_0.add();
        a1_main.a2_0.a3_0.a4_0.a6_0.add();
//         a1_main.a2_0.a3_0.a4_0.a6_0.a5_0.add();
//         a1_main.a2_0.a3_0.a4_0.a6_0.a4_0.add();
//         a1_main.a2_0.a3_0.a4_0.a6_0.a1_0.add();

        a5_main.a3_0.add();
        a5_main.a3_0.a4_0.add();

//         System.out.println("a1_main.sum:"+a1_main.sum);
//         System.out.println("a4_main.sum:"+a5_main.sum);
//         System.out.println("a1_main.a2_0.sum:"+a1_main.a2_0.sum);
//         System.out.println("a1_main.a2_0.a3_0.sum:"+a1_main.a2_0.a3_0.sum);
//         System.out.println("a1_main.a2_0.a3_0.a4_0.sum:"+a1_main.a2_0.a3_0.a4_0.sum);
//         System.out.println("a1_main.a2_0.a3_0.a4_0.a6_0.sum:"+a1_main.a2_0.a3_0.a4_0.a6_0.sum);
//         System.out.println("a1_main.a2_0.a3_0.a4_0.a6_0.a4_0.sum:"+a1_main.a2_0.a3_0.a4_0.a6_0.a4_0.sum);


//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a5_main.sum+a1_main.a2_0.sum+a1_main.a2_0.a3_0.sum+a1_main.a2_0.a3_0.a4_0.sum+a1_main.a2_0.a3_0.a4_0.a6_0.sum);

        int result = a1_main.sum + a5_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a4_0.sum + a1_main.a2_0.a3_0.a4_0.a6_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1550)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00480_A1 {
        Cycle_a_00480_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00480_A1(String strObjectName) {
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

    class Cycle_a_00480_A2 {
        Cycle_a_00480_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00480_A2(String strObjectName) {
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

    class Cycle_a_00480_A3 {
        Cycle_a_00480_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00480_A3(String strObjectName) {
            a4_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00480_A4 {
        Cycle_a_00480_A1 a1_0;
        Cycle_a_00480_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00480_A4(String strObjectName) {
            a1_0 = null;
            a6_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a6_0.a;
        }
    }

    class Cycle_a_00480_A5 {
        Cycle_a_00480_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00480_A5(String strObjectName) {
            a3_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_a_00480_A6 {
        Cycle_a_00480_A1 a1_0;
        Cycle_a_00480_A4 a4_0;
        Cycle_a_00480_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00480_A6(String strObjectName) {
            a1_0 = null;
            a4_0 = null;
            a5_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a4_0.a + a5_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00490B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00490_A1 a1_main = new Cycle_a_00490_A1("a1_main");
        Cycle_a_00490_A4 a4_main = new Cycle_a_00490_A4("a4_main");
        Cycle_a_00490_A7 a7_main = new Cycle_a_00490_A7("a7_main");
        a1_main.a2_0 = new Cycle_a_00490_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00490_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;

        a4_main.a5_0 = new Cycle_a_00490_A5("a5_0");
        a4_main.a5_0.a6_0 = new Cycle_a_00490_A6("a6_0");
        a4_main.a5_0.a6_0.a4_0 = a4_main;
//         a4_main.a5_0.a6_0.a3_0 = new Cycle_a_00490_A3("a3_0");
        a4_main.a5_0.a6_0.a3_0 = a1_main.a2_0.a3_0;
        a4_main.a5_0.a6_0.a3_0.a1_0 = a1_main;

        a7_main.a9_0 = new Cycle_a_00490_A9("a9_0");
        a7_main.a9_0.a8_0 = new Cycle_a_00490_A8("a8_0");
        a7_main.a9_0.a8_0.a7_0 = a7_main;
        a7_main.a9_0.a8_0.a5_0 = a4_main.a5_0;

        a1_main.add();
        a4_main.add();
        a7_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a1_0.add();
        a4_main.a5_0.add();
        a4_main.a5_0.a6_0.add();
        a4_main.a5_0.a6_0.a4_0.add();
        a4_main.a5_0.a6_0.a3_0.add();
        a4_main.a5_0.a6_0.a3_0.a1_0.add();

        a7_main.a9_0.add();
        a7_main.a9_0.a8_0.add();
        a7_main.a9_0.a8_0.a7_0.add();
        a7_main.a9_0.a8_0.a5_0.add();
//
//         System.out.println("a1_main.sum:"+a1_main.sum);
//         System.out.println("a4_main.sum:"+a4_main.sum);
//         System.out.println("a7_main.sum:"+a7_main.sum);
//         System.out.println("a1_main.a2_0.sum:"+a1_main.a2_0.sum);
//         System.out.println("a1_main.a2_0.a3_0.sum:"+a1_main.a2_0.a3_0.sum);
//         System.out.println("a4_main.a5_0.sum:"+a4_main.a5_0.sum);
//         System.out.println("a4_main.a5_0.a6_0.sum:"+a4_main.a5_0.a6_0.sum);
//         System.out.println("a7_main.a9_0.sum:"+a7_main.a9_0.sum);
//         System.out.println("a7_main.a9_0.a8_0.sum:"+a7_main.a9_0.a8_0.sum);


//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a4_main.sum+a7_main.sum+a1_main.a2_0.sum+a1_main.a2_0.a3_0.sum+a4_main.a5_0.sum+a4_main.a5_0.a6_0.sum+a7_main.a9_0.sum+a7_main.a9_0.a8_0.sum);

        int result = a1_main.sum + a4_main.sum + a7_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a4_main.a5_0.sum + a4_main.a5_0.a6_0.sum + a7_main.a9_0.sum + a7_main.a9_0.a8_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2098)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00490_A1 {
        Cycle_a_00490_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A1(String strObjectName) {
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

    class Cycle_a_00490_A2 {
        Cycle_a_00490_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A2(String strObjectName) {
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

    class Cycle_a_00490_A3 {
        Cycle_a_00490_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A3(String strObjectName) {
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

    class Cycle_a_00490_A4 {
        Cycle_a_00490_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A4(String strObjectName) {
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

    class Cycle_a_00490_A5 {
        Cycle_a_00490_A6 a6_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A5(String strObjectName) {
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

    class Cycle_a_00490_A6 {
        Cycle_a_00490_A3 a3_0;
        Cycle_a_00490_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A6(String strObjectName) {
            a3_0 = null;
            a4_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a3_0.a + a4_0.a;
        }
    }

    class Cycle_a_00490_A7 {
        Cycle_a_00490_A9 a9_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A7(String strObjectName) {
            a9_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a9_0.a;
        }
    }

    class Cycle_a_00490_A8 {
        Cycle_a_00490_A5 a5_0;
        Cycle_a_00490_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A8(String strObjectName) {
            a5_0 = null;
            a7_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a + a5_0.a;
        }
    }

    class Cycle_a_00490_A9 {
        Cycle_a_00490_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00490_A9(String strObjectName) {
            a8_0 = null;
            a = 109;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A9_"+strObjectName);
        }

        void add() {
            sum = a + a8_0.a;
        }
    }
}

class ThreadRc_Cycle_am_00500B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_a_00500_A1 a1_main = new Cycle_a_00500_A1("a1_main");
        Cycle_a_00500_A4 a4_main = new Cycle_a_00500_A4("a4_main");
        Cycle_a_00500_A7 a7_main = new Cycle_a_00500_A7("a7_main");
        a1_main.a2_0 = new Cycle_a_00500_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00500_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;

        a4_main.a1_0 = a1_main;
        a4_main.a5_0 = new Cycle_a_00500_A5("a5_0");
        a4_main.a5_0.a6_0 = new Cycle_a_00500_A6("a6_0");
        a4_main.a5_0.a6_0.a4_0 = a4_main;

        a4_main.a5_0.a8_0 = new Cycle_a_00500_A8("a8_0");
        a4_main.a5_0.a8_0.a7_0 = a7_main;

        a7_main.a9_0 = new Cycle_a_00500_A9("a9_0");
        a7_main.a9_0.a8_0 = new Cycle_a_00500_A8("a8_0");
        a7_main.a9_0.a8_0.a7_0 = a7_main;

        a1_main.add();
        a4_main.add();
        a7_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a1_0.add();
        a4_main.a1_0.add();
        a4_main.a5_0.add();
        a4_main.a5_0.a6_0.add();
        a4_main.a5_0.a6_0.a4_0.add();
        a4_main.a5_0.a8_0.add();
        a4_main.a5_0.a8_0.a7_0.add();
        a7_main.a9_0.add();
        a7_main.a9_0.a8_0.add();
        a7_main.a9_0.a8_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a7_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a4_main.a5_0.sum + a4_main.a5_0.a6_0.sum + a7_main.a9_0.sum + a7_main.a9_0.a8_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2099)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_a_00500_A1 {
        Cycle_a_00500_A2 a2_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A1(String strObjectName) {
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

    class Cycle_a_00500_A2 {
        Cycle_a_00500_A3 a3_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A2(String strObjectName) {
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

    class Cycle_a_00500_A3 {
        Cycle_a_00500_A1 a1_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A3(String strObjectName) {
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

    class Cycle_a_00500_A4 {
        Cycle_a_00500_A1 a1_0;
        Cycle_a_00500_A5 a5_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A4(String strObjectName) {
            a1_0 = null;
            a5_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + a1_0.a + a5_0.a;
        }
    }

    class Cycle_a_00500_A5 {
        Cycle_a_00500_A6 a6_0;
        Cycle_a_00500_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A5(String strObjectName) {
            a6_0 = null;
            a8_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + a6_0.a + a8_0.a;
        }
    }

    class Cycle_a_00500_A6 {
        Cycle_a_00500_A4 a4_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A6(String strObjectName) {
            a4_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + a4_0.a;
        }
    }

    class Cycle_a_00500_A7 {
        Cycle_a_00500_A9 a9_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A7(String strObjectName) {
            a9_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + a9_0.a;
        }
    }

    class Cycle_a_00500_A8 {
        Cycle_a_00500_A7 a7_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A8(String strObjectName) {
            a7_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + a7_0.a;
        }
    }

    class Cycle_a_00500_A9 {
        Cycle_a_00500_A8 a8_0;
        int a;
        int sum;
        String strObjectName;

        Cycle_a_00500_A9(String strObjectName) {
            a8_0 = null;
            a = 109;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A9_"+strObjectName);
        }

        void add() {
            sum = a + a8_0.a;
        }
    }
}

public class Cycle_am_00510 {
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
        ThreadRc_Cycle_am_00180B A1_Cycle_am_00180 = new ThreadRc_Cycle_am_00180B();
        ThreadRc_Cycle_am_00180B A2_Cycle_am_00180 = new ThreadRc_Cycle_am_00180B();
        ThreadRc_Cycle_am_00190B A1_Cycle_am_00190 = new ThreadRc_Cycle_am_00190B();
        ThreadRc_Cycle_am_00190B A2_Cycle_am_00190 = new ThreadRc_Cycle_am_00190B();
        ThreadRc_Cycle_am_00200B A1_Cycle_am_00200 = new ThreadRc_Cycle_am_00200B();
        ThreadRc_Cycle_am_00200B A2_Cycle_am_00200 = new ThreadRc_Cycle_am_00200B();
        ThreadRc_Cycle_am_00210B A1_Cycle_am_00210 = new ThreadRc_Cycle_am_00210B();
        ThreadRc_Cycle_am_00210B A2_Cycle_am_00210 = new ThreadRc_Cycle_am_00210B();
        ThreadRc_Cycle_am_00220B A1_Cycle_am_00220 = new ThreadRc_Cycle_am_00220B();
        ThreadRc_Cycle_am_00220B A2_Cycle_am_00220 = new ThreadRc_Cycle_am_00220B();
        ThreadRc_Cycle_am_00230B A1_Cycle_am_00230 = new ThreadRc_Cycle_am_00230B();
        ThreadRc_Cycle_am_00230B A2_Cycle_am_00230 = new ThreadRc_Cycle_am_00230B();
        ThreadRc_Cycle_am_00240B A1_Cycle_am_00240 = new ThreadRc_Cycle_am_00240B();
        ThreadRc_Cycle_am_00240B A2_Cycle_am_00240 = new ThreadRc_Cycle_am_00240B();
        ThreadRc_Cycle_am_00250B A1_Cycle_am_00250 = new ThreadRc_Cycle_am_00250B();
        ThreadRc_Cycle_am_00250B A2_Cycle_am_00250 = new ThreadRc_Cycle_am_00250B();
        ThreadRc_Cycle_am_00260B A1_Cycle_am_00260 = new ThreadRc_Cycle_am_00260B();
        ThreadRc_Cycle_am_00260B A2_Cycle_am_00260 = new ThreadRc_Cycle_am_00260B();
        ThreadRc_Cycle_am_00270B A1_Cycle_am_00270 = new ThreadRc_Cycle_am_00270B();
        ThreadRc_Cycle_am_00270B A2_Cycle_am_00270 = new ThreadRc_Cycle_am_00270B();
        ThreadRc_Cycle_am_00280B A1_Cycle_am_00280 = new ThreadRc_Cycle_am_00280B();
        ThreadRc_Cycle_am_00280B A2_Cycle_am_00280 = new ThreadRc_Cycle_am_00280B();
        ThreadRc_Cycle_am_00290B A1_Cycle_am_00290 = new ThreadRc_Cycle_am_00290B();
        ThreadRc_Cycle_am_00290B A2_Cycle_am_00290 = new ThreadRc_Cycle_am_00290B();
        ThreadRc_Cycle_am_00300B A1_Cycle_am_00300 = new ThreadRc_Cycle_am_00300B();
        ThreadRc_Cycle_am_00300B A2_Cycle_am_00300 = new ThreadRc_Cycle_am_00300B();
        ThreadRc_Cycle_am_00310B A1_Cycle_am_00310 = new ThreadRc_Cycle_am_00310B();
        ThreadRc_Cycle_am_00310B A2_Cycle_am_00310 = new ThreadRc_Cycle_am_00310B();
        ThreadRc_Cycle_am_00320B A1_Cycle_am_00320 = new ThreadRc_Cycle_am_00320B();
        ThreadRc_Cycle_am_00320B A2_Cycle_am_00320 = new ThreadRc_Cycle_am_00320B();
        ThreadRc_Cycle_am_00330B A1_Cycle_am_00330 = new ThreadRc_Cycle_am_00330B();
        ThreadRc_Cycle_am_00330B A2_Cycle_am_00330 = new ThreadRc_Cycle_am_00330B();
        ThreadRc_Cycle_am_00340B A1_Cycle_am_00340 = new ThreadRc_Cycle_am_00340B();
        ThreadRc_Cycle_am_00340B A2_Cycle_am_00340 = new ThreadRc_Cycle_am_00340B();
        ThreadRc_Cycle_am_00350B A1_Cycle_am_00350 = new ThreadRc_Cycle_am_00350B();
        ThreadRc_Cycle_am_00350B A2_Cycle_am_00350 = new ThreadRc_Cycle_am_00350B();
        ThreadRc_Cycle_am_00360B A1_Cycle_am_00360 = new ThreadRc_Cycle_am_00360B();
        ThreadRc_Cycle_am_00360B A2_Cycle_am_00360 = new ThreadRc_Cycle_am_00360B();
        ThreadRc_Cycle_am_00370B A1_Cycle_am_00370 = new ThreadRc_Cycle_am_00370B();
        ThreadRc_Cycle_am_00370B A2_Cycle_am_00370 = new ThreadRc_Cycle_am_00370B();
        ThreadRc_Cycle_am_00380B A1_Cycle_am_00380 = new ThreadRc_Cycle_am_00380B();
        ThreadRc_Cycle_am_00380B A2_Cycle_am_00380 = new ThreadRc_Cycle_am_00380B();
        ThreadRc_Cycle_am_00390B A1_Cycle_am_00390 = new ThreadRc_Cycle_am_00390B();
        ThreadRc_Cycle_am_00390B A2_Cycle_am_00390 = new ThreadRc_Cycle_am_00390B();
        ThreadRc_Cycle_am_00400B A1_Cycle_am_00400 = new ThreadRc_Cycle_am_00400B();
        ThreadRc_Cycle_am_00400B A2_Cycle_am_00400 = new ThreadRc_Cycle_am_00400B();
        ThreadRc_Cycle_am_00410B A1_Cycle_am_00410 = new ThreadRc_Cycle_am_00410B();
        ThreadRc_Cycle_am_00410B A2_Cycle_am_00410 = new ThreadRc_Cycle_am_00410B();
        ThreadRc_Cycle_am_00420B A1_Cycle_am_00420 = new ThreadRc_Cycle_am_00420B();
        ThreadRc_Cycle_am_00420B A2_Cycle_am_00420 = new ThreadRc_Cycle_am_00420B();
        ThreadRc_Cycle_am_00430B A1_Cycle_am_00430 = new ThreadRc_Cycle_am_00430B();
        ThreadRc_Cycle_am_00430B A2_Cycle_am_00430 = new ThreadRc_Cycle_am_00430B();
        ThreadRc_Cycle_am_00440B A1_Cycle_am_00440 = new ThreadRc_Cycle_am_00440B();
        ThreadRc_Cycle_am_00440B A2_Cycle_am_00440 = new ThreadRc_Cycle_am_00440B();
        ThreadRc_Cycle_am_00450B A1_Cycle_am_00450 = new ThreadRc_Cycle_am_00450B();
        ThreadRc_Cycle_am_00450B A2_Cycle_am_00450 = new ThreadRc_Cycle_am_00450B();
        ThreadRc_Cycle_am_00460B A1_Cycle_am_00460 = new ThreadRc_Cycle_am_00460B();
        ThreadRc_Cycle_am_00460B A2_Cycle_am_00460 = new ThreadRc_Cycle_am_00460B();
        ThreadRc_Cycle_am_00470B A1_Cycle_am_00470 = new ThreadRc_Cycle_am_00470B();
        ThreadRc_Cycle_am_00470B A2_Cycle_am_00470 = new ThreadRc_Cycle_am_00470B();
        ThreadRc_Cycle_am_00480B A1_Cycle_am_00480 = new ThreadRc_Cycle_am_00480B();
        ThreadRc_Cycle_am_00480B A2_Cycle_am_00480 = new ThreadRc_Cycle_am_00480B();
        ThreadRc_Cycle_am_00490B A1_Cycle_am_00490 = new ThreadRc_Cycle_am_00490B();
        ThreadRc_Cycle_am_00490B A2_Cycle_am_00490 = new ThreadRc_Cycle_am_00490B();
        ThreadRc_Cycle_am_00500B A1_Cycle_am_00500 = new ThreadRc_Cycle_am_00500B();
        ThreadRc_Cycle_am_00500B A2_Cycle_am_00500 = new ThreadRc_Cycle_am_00500B();
        A1_Cycle_am_00180.start();
        A2_Cycle_am_00180.start();
        A1_Cycle_am_00190.start();
        A2_Cycle_am_00190.start();
        A1_Cycle_am_00200.start();
        A2_Cycle_am_00200.start();
        A1_Cycle_am_00210.start();
        A2_Cycle_am_00210.start();
        A1_Cycle_am_00220.start();
        A2_Cycle_am_00220.start();
        A1_Cycle_am_00230.start();
        A2_Cycle_am_00230.start();
        A1_Cycle_am_00240.start();
        A2_Cycle_am_00240.start();
        A1_Cycle_am_00250.start();
        A2_Cycle_am_00250.start();
        A1_Cycle_am_00260.start();
        A2_Cycle_am_00260.start();
        A1_Cycle_am_00270.start();
        A2_Cycle_am_00270.start();
        A1_Cycle_am_00280.start();
        A2_Cycle_am_00280.start();
        A1_Cycle_am_00290.start();
        A2_Cycle_am_00290.start();
        A1_Cycle_am_00300.start();
        A2_Cycle_am_00300.start();
        A1_Cycle_am_00310.start();
        A2_Cycle_am_00310.start();
        A1_Cycle_am_00320.start();
        A2_Cycle_am_00320.start();
        A1_Cycle_am_00330.start();
        A2_Cycle_am_00330.start();
        A1_Cycle_am_00340.start();
        A2_Cycle_am_00340.start();
        A1_Cycle_am_00350.start();
        A2_Cycle_am_00350.start();
        A1_Cycle_am_00360.start();
        A2_Cycle_am_00360.start();
        A1_Cycle_am_00370.start();
        A2_Cycle_am_00370.start();
        A1_Cycle_am_00380.start();
        A2_Cycle_am_00380.start();
        A1_Cycle_am_00390.start();
        A2_Cycle_am_00390.start();
        A1_Cycle_am_00400.start();
        A2_Cycle_am_00400.start();
        A1_Cycle_am_00410.start();
        A2_Cycle_am_00410.start();
        A1_Cycle_am_00420.start();
        A2_Cycle_am_00420.start();
        A1_Cycle_am_00430.start();
        A2_Cycle_am_00430.start();
        A1_Cycle_am_00440.start();
        A2_Cycle_am_00440.start();
        A1_Cycle_am_00450.start();
        A2_Cycle_am_00450.start();
        A1_Cycle_am_00460.start();
        A2_Cycle_am_00460.start();
        A1_Cycle_am_00470.start();
        A2_Cycle_am_00470.start();
        A1_Cycle_am_00480.start();
        A2_Cycle_am_00480.start();
        A1_Cycle_am_00490.start();
        A2_Cycle_am_00490.start();
        A1_Cycle_am_00500.start();
        A2_Cycle_am_00500.start();
        try {
            A1_Cycle_am_00180.join();
            A2_Cycle_am_00180.join();
            A1_Cycle_am_00190.join();
            A2_Cycle_am_00190.join();
            A1_Cycle_am_00200.join();
            A2_Cycle_am_00200.join();
            A1_Cycle_am_00210.join();
            A2_Cycle_am_00210.join();
            A1_Cycle_am_00220.join();
            A2_Cycle_am_00220.join();
            A1_Cycle_am_00230.join();
            A2_Cycle_am_00230.join();
            A1_Cycle_am_00240.join();
            A2_Cycle_am_00240.join();
            A1_Cycle_am_00250.join();
            A2_Cycle_am_00250.join();
            A1_Cycle_am_00260.join();
            A2_Cycle_am_00260.join();
            A1_Cycle_am_00270.join();
            A2_Cycle_am_00270.join();
            A1_Cycle_am_00280.join();
            A2_Cycle_am_00280.join();
            A1_Cycle_am_00290.join();
            A2_Cycle_am_00290.join();
            A1_Cycle_am_00300.join();
            A2_Cycle_am_00300.join();
            A1_Cycle_am_00310.join();
            A2_Cycle_am_00310.join();
            A1_Cycle_am_00320.join();
            A2_Cycle_am_00320.join();
            A1_Cycle_am_00330.join();
            A2_Cycle_am_00330.join();
            A1_Cycle_am_00340.join();
            A2_Cycle_am_00340.join();
            A1_Cycle_am_00350.join();
            A2_Cycle_am_00350.join();
            A1_Cycle_am_00360.join();
            A2_Cycle_am_00360.join();
            A1_Cycle_am_00370.join();
            A2_Cycle_am_00370.join();
            A1_Cycle_am_00380.join();
            A2_Cycle_am_00380.join();
            A1_Cycle_am_00390.join();
            A2_Cycle_am_00390.join();
            A1_Cycle_am_00400.join();
            A2_Cycle_am_00400.join();
            A1_Cycle_am_00410.join();
            A2_Cycle_am_00410.join();
            A1_Cycle_am_00420.join();
            A2_Cycle_am_00420.join();
            A1_Cycle_am_00430.join();
            A2_Cycle_am_00430.join();
            A1_Cycle_am_00440.join();
            A2_Cycle_am_00440.join();
            A1_Cycle_am_00450.join();
            A2_Cycle_am_00450.join();
            A1_Cycle_am_00460.join();
            A2_Cycle_am_00460.join();
            A1_Cycle_am_00470.join();
            A2_Cycle_am_00470.join();
            A1_Cycle_am_00480.join();
            A2_Cycle_am_00480.join();
            A1_Cycle_am_00490.join();
            A2_Cycle_am_00490.join();
            A1_Cycle_am_00500.join();
            A2_Cycle_am_00500.join();
        } catch (InterruptedException e) {
        }
        if (A1_Cycle_am_00180.check() && A2_Cycle_am_00180.check() && A1_Cycle_am_00190.check() && A2_Cycle_am_00190.check() && A1_Cycle_am_00200.check() && A2_Cycle_am_00200.check() && A1_Cycle_am_00210.check() && A2_Cycle_am_00210.check() && A1_Cycle_am_00220.check() && A2_Cycle_am_00220.check() && A1_Cycle_am_00230.check() && A2_Cycle_am_00230.check() && A1_Cycle_am_00240.check() && A2_Cycle_am_00240.check() && A1_Cycle_am_00250.check() && A2_Cycle_am_00250.check() && A1_Cycle_am_00260.check() && A2_Cycle_am_00260.check() && A1_Cycle_am_00270.check() && A2_Cycle_am_00270.check() && A1_Cycle_am_00280.check() && A2_Cycle_am_00280.check() && A1_Cycle_am_00290.check() && A2_Cycle_am_00290.check() && A1_Cycle_am_00300.check() && A2_Cycle_am_00300.check() && A1_Cycle_am_00310.check() && A2_Cycle_am_00310.check() && A1_Cycle_am_00320.check() && A2_Cycle_am_00320.check() && A1_Cycle_am_00330.check() && A2_Cycle_am_00330.check() && A1_Cycle_am_00340.check() && A2_Cycle_am_00340.check() && A1_Cycle_am_00350.check() && A2_Cycle_am_00350.check() && A1_Cycle_am_00360.check() && A2_Cycle_am_00360.check() && A1_Cycle_am_00370.check() && A2_Cycle_am_00370.check() && A1_Cycle_am_00380.check() && A2_Cycle_am_00380.check() && A1_Cycle_am_00390.check() && A2_Cycle_am_00390.check() && A1_Cycle_am_00400.check() && A2_Cycle_am_00400.check() && A1_Cycle_am_00410.check() && A2_Cycle_am_00410.check() && A1_Cycle_am_00420.check() && A2_Cycle_am_00420.check() && A1_Cycle_am_00430.check() && A2_Cycle_am_00430.check() && A1_Cycle_am_00440.check() && A2_Cycle_am_00440.check() && A1_Cycle_am_00450.check() && A2_Cycle_am_00450.check() && A1_Cycle_am_00460.check() && A2_Cycle_am_00460.check() && A1_Cycle_am_00470.check() && A2_Cycle_am_00470.check() && A1_Cycle_am_00480.check() && A2_Cycle_am_00480.check() && A1_Cycle_am_00490.check() && A2_Cycle_am_00490.check() && A1_Cycle_am_00500.check() && A2_Cycle_am_00500.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\nExpectResult\nExpectResult\n