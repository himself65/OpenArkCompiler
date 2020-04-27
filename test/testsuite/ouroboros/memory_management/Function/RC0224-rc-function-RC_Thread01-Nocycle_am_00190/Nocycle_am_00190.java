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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Nocycle_am_00190.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:make Nocycle_a_xx together to this Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_am_00190.java
 *- @ExecuteClass: Nocycle_am_00190
 *- @ExecuteArgs:
 */
class ThreadRc_00010B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_am_00010_A1 a1_main = new Nocycle_am_00010_A1("a1_main");
        a1_main.b1_0 = new Nocycle_am_00010_B1("b1_0");
        a1_main.add();
        a1_main.b1_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum);
        int result = a1_main.sum + a1_main.b1_0.sum;
        //System.out.println("RC-Testing_Result="+result);
        if (result == 704)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_am_00010_A1 {
        Nocycle_am_00010_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_am_00010_A1(String strObjectName) {
            b1_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        //   protected void finalize() throws java.lang.Throwable {
//       System.out.println("RC-Testing_Destruction_A1_"+strObjectName);
//   }
        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_am_00010_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_am_00010_B1(String strObjectName) {
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

class ThreadRc_00020B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00020_A1 a1_main = new Nocycle_a_00020_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00020_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00020_B2("b2_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum);
        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum;
        //System.out.println("RC-Testing_Result="+result);
        if (result == 1310)
            checkout = true;
        //System.out.println(checkout);

    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00020_A1 {
        Nocycle_a_00020_B1 b1_0;
        Nocycle_a_00020_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00020_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00020_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00020_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00020_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00020_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00030B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00030_A1 a1_main = new Nocycle_a_00030_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00030_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00030_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_00030_B3("b3_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum);
        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum;
        //System.out.println("RC-Testing_Result="+result);
        if (result == 1919)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00030_A1 {
        Nocycle_a_00030_B1 b1_0;
        Nocycle_a_00030_B2 b2_0;
        Nocycle_a_00030_B3 b3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00030_A1(String strObjectName) {
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

    class Nocycle_a_00030_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00030_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00030_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00030_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00030_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00030_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00040B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00040_A1 a1_main = new Nocycle_a_00040_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00040_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00040_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_00040_B3("b3_0");
        a1_main.b4_0 = new Nocycle_a_00040_B4("b4_0");
        a1_main.b5_0 = new Nocycle_a_00040_B5("b5_0");
        a1_main.b6_0 = new Nocycle_a_00040_B6("b6_0");
        a1_main.b7_0 = new Nocycle_a_00040_B7("b7_0");
        a1_main.b8_0 = new Nocycle_a_00040_B8("b8_0");
        a1_main.b9_0 = new Nocycle_a_00040_B9("b9_0");
        a1_main.b10_0 = new Nocycle_a_00040_B10("b10_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a1_main.b4_0.add();
        a1_main.b5_0.add();
        a1_main.b6_0.add();
        a1_main.b7_0.add();
        a1_main.b8_0.add();
        a1_main.b9_0.add();
        a1_main.b10_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum+a1_main.b4_0.sum+a1_main.b5_0.sum+a1_main.b6_0.sum+a1_main.b7_0.sum+a1_main.b8_0.sum+a1_main.b9_0.sum+a1_main.b10_0.sum);

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a1_main.b4_0.sum + a1_main.b5_0.sum + a1_main.b6_0.sum + a1_main.b7_0.sum + a1_main.b8_0.sum + a1_main.b9_0.sum + a1_main.b10_0.sum;
        //System.out.println("RC-Testing_Result="+result);
        if (result == 6266)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00040_A1 {
        Nocycle_a_00040_B1 b1_0;
        Nocycle_a_00040_B2 b2_0;
        Nocycle_a_00040_B3 b3_0;
        Nocycle_a_00040_B4 b4_0;
        Nocycle_a_00040_B5 b5_0;
        Nocycle_a_00040_B6 b6_0;
        Nocycle_a_00040_B7 b7_0;
        Nocycle_a_00040_B8 b8_0;
        Nocycle_a_00040_B9 b9_0;
        Nocycle_a_00040_B10 b10_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            b3_0 = null;
            b4_0 = null;
            b5_0 = null;
            b6_0 = null;
            b7_0 = null;
            b8_0 = null;
            b9_0 = null;
            b10_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a + b3_0.a + b4_0.a + b5_0.a + b6_0.a + b7_0.a + b8_0.a + b9_0.a + b10_0.a;
        }
    }

    class Nocycle_a_00040_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B4 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B4(String strObjectName) {
            a = 204;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B5 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B5(String strObjectName) {
            a = 205;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B5_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B6 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B6(String strObjectName) {
            a = 206;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B6_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B7 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B7(String strObjectName) {
            a = 207;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B7_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B8 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B8(String strObjectName) {
            a = 208;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B8_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B9 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B9(String strObjectName) {
            a = 209;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B9_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00040_B10 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00040_B10(String strObjectName) {
            a = 210;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B10_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00050B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00050_A1 a1_main = new Nocycle_a_00050_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00050_B1("b1_0");
        a1_main.b1_0.c1_0 = new Nocycle_a_00050_C1("c1_0");
        a1_main.b1_0.c2_0 = new Nocycle_a_00050_C2("c2_0");
        a1_main.b1_0.c3_0 = new Nocycle_a_00050_C3("c3_0");
        a1_main.b2_0 = new Nocycle_a_00050_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_00050_B3("b3_0");
        a1_main.b4_0 = new Nocycle_a_00050_B4("b4_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a1_main.b4_0.add();
        a1_main.b1_0.c1_0.add();
        a1_main.b1_0.c2_0.add();
        a1_main.b1_0.c3_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum+a1_main.b4_0.sum+a1_main.b1_0.c1_0.sum+a1_main.b1_0.c2_0.sum+a1_main.b1_0.c3_0.sum);

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a1_main.b4_0.sum + a1_main.b1_0.c1_0.sum + a1_main.b1_0.c2_0.sum + a1_main.b1_0.c3_0.sum;
        //System.out.println("RC-Testing_Result="+result);
        if (result == 5048)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00050_A1 {
        Nocycle_a_00050_B1 b1_0;
        Nocycle_a_00050_B2 b2_0;
        Nocycle_a_00050_B3 b3_0;
        Nocycle_a_00050_B4 b4_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_A1(String strObjectName) {
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

    class Nocycle_a_00050_B1 {
        Nocycle_a_00050_C1 c1_0;
        Nocycle_a_00050_C2 c2_0;
        Nocycle_a_00050_C3 c3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_B1(String strObjectName) {
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

    class Nocycle_a_00050_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00050_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00050_B4 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_B4(String strObjectName) {
            a = 204;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00050_C1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_C1(String strObjectName) {
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00050_C2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_C2(String strObjectName) {
            a = 302;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00050_C3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00050_C3(String strObjectName) {
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

class ThreadRc_00060B extends Thread {
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
        //System.out.println("RC-Testing_Result="+result);
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

class ThreadRc_00070B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00070_A1 a1_main = new Nocycle_a_00070_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00070_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00070_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_00070_B3("b3_0");
        a1_main.b4_0 = new Nocycle_a_00070_B4("b4_0");
        a1_main.b4_0.c1_0 = new Nocycle_a_00070_C1("c1_0");
        a1_main.b4_0.c2_0 = new Nocycle_a_00070_C2("c2_0");
        a1_main.b4_0.c3_0 = new Nocycle_a_00070_C3("c3_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a1_main.b4_0.add();
        a1_main.b4_0.c1_0.add();
        a1_main.b4_0.c2_0.add();
        a1_main.b4_0.c3_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum+a1_main.b4_0.sum+a1_main.b4_0.c1_0.sum+a1_main.b4_0.c2_0.sum+a1_main.b4_0.c3_0.sum);

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a1_main.b4_0.sum + a1_main.b4_0.c1_0.sum + a1_main.b4_0.c2_0.sum + a1_main.b4_0.c3_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 5045)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00070_A1 {
        Nocycle_a_00070_B1 b1_0;
        Nocycle_a_00070_B2 b2_0;
        Nocycle_a_00070_B3 b3_0;
        Nocycle_a_00070_B4 b4_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_A1(String strObjectName) {
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

    class Nocycle_a_00070_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00070_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00070_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00070_B4 {
        Nocycle_a_00070_C1 c1_0;
        Nocycle_a_00070_C2 c2_0;
        Nocycle_a_00070_C3 c3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_B4(String strObjectName) {
            c1_0 = null;
            c2_0 = null;
            c3_0 = null;
            a = 204;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a + c2_0.a + c3_0.a;
        }
    }

    class Nocycle_a_00070_C1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_C1(String strObjectName) {
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00070_C2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_C2(String strObjectName) {
            a = 302;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00070_C3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00070_C3(String strObjectName) {
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

class ThreadRc_00080B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00080_A1 a1_main = new Nocycle_a_00080_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00080_B1("b1_0");
        a1_main.b1_0.c1_0 = new Nocycle_a_00080_C1("c1_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b1_0.c1_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b1_0.c1_0.sum);

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b1_0.c1_0.sum;
        //System.out.println("RC-Testing_Result="+result);
        if (result == 1406)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00080_A1 {
        Nocycle_a_00080_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00080_A1(String strObjectName) {
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

    class Nocycle_a_00080_B1 {
        Nocycle_a_00080_C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00080_B1(String strObjectName) {
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

    class Nocycle_a_00080_C1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00080_C1(String strObjectName) {
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00090B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00090_A1 a1_main = new Nocycle_a_00090_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00090_B1("b1_0");
        a1_main.b1_0.c1_0 = new Nocycle_a_00090_C1("c1_0");
        a1_main.b1_0.c1_0.d1_0 = new Nocycle_a_00090_D1("d1_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b1_0.c1_0.add();
        a1_main.b1_0.c1_0.d1_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b1_0.c1_0.sum+a1_main.b1_0.c1_0.d1_0.sum);

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b1_0.c1_0.sum + a1_main.b1_0.c1_0.d1_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2308)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00090_A1 {
        Nocycle_a_00090_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00090_A1(String strObjectName) {
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

    class Nocycle_a_00090_B1 {
        Nocycle_a_00090_C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00090_B1(String strObjectName) {
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

    class Nocycle_a_00090_C1 {
        Nocycle_a_00090_D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00090_C1(String strObjectName) {
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

    class Nocycle_a_00090_D1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00090_D1(String strObjectName) {
            a = 401;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00100B extends Thread {
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

class ThreadRc_00110B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00110_A1 a1_main = new Nocycle_a_00110_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_00110_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00110_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_00110_B3("b3_0");
        a1_main.b4_0 = new Nocycle_a_00110_B4("b4_0");
        a1_main.b1_0.d1_0 = new Nocycle_a_00110_D1("d1_0");
        a1_main.b2_0.c2_0 = new Nocycle_a_00110_C2("c2_0");
        a1_main.b3_0.c1_0 = new Nocycle_a_00110_C1("c1_0");
        a1_main.b4_0.c3_0 = new Nocycle_a_00110_C3("c3_0");
        a1_main.b3_0.c1_0.d2_0 = new Nocycle_a_00110_D2("d2_0");
        a1_main.b2_0.c2_0.d3_0 = new Nocycle_a_00110_D3("d3_0");
        a1_main.b2_0.c2_0.d4_0 = new Nocycle_a_00110_D4("d4_0");
        a1_main.d5_0 = new Nocycle_a_00110_D5("d5_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a1_main.b4_0.add();
        a1_main.b2_0.c2_0.add();
        a1_main.b3_0.c1_0.add();
        a1_main.b4_0.c3_0.add();
        a1_main.b1_0.d1_0.add();
        a1_main.b3_0.c1_0.d2_0.add();
        a1_main.b2_0.c2_0.d3_0.add();
        a1_main.b2_0.c2_0.d4_0.add();
        a1_main.d5_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum+a1_main.b4_0.sum+a1_main.b2_0.c2_0.sum+a1_main.b3_0.c1_0.sum+a1_main.b4_0.c3_0.sum+a1_main.b1_0.d1_0.sum+a1_main.b3_0.c1_0.d2_0.sum+a1_main.b2_0.c2_0.d3_0.sum+a1_main.b2_0.c2_0.d4_0.sum+a1_main.d5_0.sum);
        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a1_main.b4_0.sum + a1_main.b2_0.c2_0.sum + a1_main.b3_0.c1_0.sum + a1_main.b4_0.c3_0.sum + a1_main.b1_0.d1_0.sum + a1_main.b3_0.c1_0.d2_0.sum + a1_main.b2_0.c2_0.d3_0.sum + a1_main.b2_0.c2_0.d4_0.sum + a1_main.d5_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 9881)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00110_A1 {
        Nocycle_a_00110_B1 b1_0;
        Nocycle_a_00110_B2 b2_0;
        Nocycle_a_00110_B3 b3_0;
        Nocycle_a_00110_B4 b4_0;
        Nocycle_a_00110_D5 d5_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            b3_0 = null;
            b4_0 = null;
            d5_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a + b3_0.a + b4_0.a + d5_0.a;
        }
    }

    class Nocycle_a_00110_B1 {
        Nocycle_a_00110_D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + d1_0.a;
        }
    }

    class Nocycle_a_00110_B2 {
        Nocycle_a_00110_C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_B2(String strObjectName) {
            c2_0 = null;
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + c2_0.a;
        }
    }

    class Nocycle_a_00110_B3 {
        Nocycle_a_00110_C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_B3(String strObjectName) {
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

    class Nocycle_a_00110_B4 {
        Nocycle_a_00110_C3 c3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_B4(String strObjectName) {
            c3_0 = null;
            a = 204;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
        }

        void add() {
            sum = a + c3_0.a;
        }
    }

    class Nocycle_a_00110_C1 {
        Nocycle_a_00110_D2 d2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_C1(String strObjectName) {
            d2_0 = null;
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + d2_0.a;
        }
    }

    class Nocycle_a_00110_C2 {
        Nocycle_a_00110_D3 d3_0;
        Nocycle_a_00110_D4 d4_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_C2(String strObjectName) {
            d3_0 = null;
            d4_0 = null;
            a = 302;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
        }

        void add() {
            sum = a + d3_0.a + d4_0.a;
        }
    }

    class Nocycle_a_00110_C3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_C3(String strObjectName) {
            a = 303;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00110_D1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_D1(String strObjectName) {
            a = 401;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00110_D2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_D2(String strObjectName) {
            a = 402;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00110_D3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_D3(String strObjectName) {
            a = 403;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00110_D4 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_D4(String strObjectName) {
            a = 404;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D4_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00110_D5 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00110_D5(String strObjectName) {
            a = 405;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D5_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00120B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00120_A1 a1_main = new Nocycle_a_00120_A1("a1_main");
        Nocycle_a_00120_A2 a2_main = new Nocycle_a_00120_A2("a2_main");
        a1_main.b1_0 = new Nocycle_a_00120_B1("b1_0");
        a2_main.b1_0 = new Nocycle_a_00120_B1("b1_0");
        a1_main.add();
        a2_main.add();
        a1_main.b1_0.add();
        a2_main.b1_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a2_main.sum+a1_main.b1_0.sum);

        int result = a1_main.sum + a2_main.sum + a1_main.b1_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1007)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00120_A1 {
        Nocycle_a_00120_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00120_A1(String strObjectName) {
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

    class Nocycle_a_00120_A2 {
        Nocycle_a_00120_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00120_A2(String strObjectName) {
            b1_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00120_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00120_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00130B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00130_A1 a1_main = new Nocycle_a_00130_A1("a1_main");
        Nocycle_a_00130_A2 a2_main = new Nocycle_a_00130_A2("a2_main");
        Nocycle_a_00130_A3 a3_main = new Nocycle_a_00130_A3("a3_main");
        Nocycle_a_00130_A4 a4_main = new Nocycle_a_00130_A4("a4_main");
        Nocycle_a_00130_A5 a5_main = new Nocycle_a_00130_A5("a5_main");
        Nocycle_a_00130_A6 a6_main = new Nocycle_a_00130_A6("a6_main");
        Nocycle_a_00130_A7 a7_main = new Nocycle_a_00130_A7("a7_main");
        Nocycle_a_00130_A8 a8_main = new Nocycle_a_00130_A8("a8_main");
        Nocycle_a_00130_A9 a9_main = new Nocycle_a_00130_A9("a9_main");
        Nocycle_a_00130_A10 a10_main = new Nocycle_a_00130_A10("a10_main");
        a1_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a2_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a3_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a4_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a5_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a6_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a7_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a8_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a9_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a10_main.b1_0 = new Nocycle_a_00130_B1("b1_0");
        a1_main.add();
        a2_main.add();
        a3_main.add();
        a4_main.add();
        a5_main.add();
        a6_main.add();
        a7_main.add();
        a8_main.add();
        a9_main.add();
        a10_main.add();
        a1_main.b1_0.add();
        a2_main.b1_0.add();
        a3_main.b1_0.add();
        a4_main.b1_0.add();
        a5_main.b1_0.add();
        a6_main.b1_0.add();
        a7_main.b1_0.add();
        a8_main.b1_0.add();
        a9_main.b1_0.add();
        a10_main.b1_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a2_main.sum+a3_main.sum+a4_main.sum+a5_main.sum+a6_main.sum+a7_main.sum+a8_main.sum+a9_main.sum+a10_main.sum+a1_main.b1_0.sum);

        int result = a1_main.sum + a2_main.sum + a3_main.sum + a4_main.sum + a5_main.sum + a6_main.sum + a7_main.sum + a8_main.sum + a9_main.sum + a10_main.sum + a1_main.b1_0.sum;
        //System.out.println("RC-Testing_Result="+result);
        if (result == 3467)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00130_A1 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A1(String strObjectName) {
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

    class Nocycle_a_00130_A2 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A2(String strObjectName) {
            b1_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A3 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A3(String strObjectName) {
            b1_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A4 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A4(String strObjectName) {
            b1_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A5 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A5(String strObjectName) {
            b1_0 = null;
            a = 105;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A6 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A6(String strObjectName) {
            b1_0 = null;
            a = 106;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A7 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A7(String strObjectName) {
            b1_0 = null;
            a = 107;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A8 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A8(String strObjectName) {
            b1_0 = null;
            a = 108;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A9 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A9(String strObjectName) {
            b1_0 = null;
            a = 109;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A9_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_A10 {
        Nocycle_a_00130_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_A10(String strObjectName) {
            b1_0 = null;
            a = 110;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A10_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00130_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00130_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00140B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00140_A1 a1_main = new Nocycle_a_00140_A1("a1_main");
        Nocycle_a_00140_A2 a2_main = new Nocycle_a_00140_A2("a2_main");
        a1_main.b1_0 = new Nocycle_a_00140_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00140_B2("b2_0");
        a2_main.b1_0 = new Nocycle_a_00140_B1("b1_0");
        a1_main.add();
        a2_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a2_main.b1_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a2_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum);

        int result = a1_main.sum + a2_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1613)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00140_A1 {
        Nocycle_a_00140_B1 b1_0;
        Nocycle_a_00140_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00140_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00140_A2 {
        Nocycle_a_00140_B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00140_A2(String strObjectName) {
            b1_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_00140_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00140_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00140_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00140_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00150B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00150_A1 a1_main = new Nocycle_a_00150_A1("a1_main");
        Nocycle_a_00150_A2 a2_main = new Nocycle_a_00150_A2("a2_main");
        a1_main.b1_0 = new Nocycle_a_00150_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00150_B2("b2_0");
        a2_main.b1_0 = new Nocycle_a_00150_B1("b1_0");
        a2_main.b2_0 = new Nocycle_a_00150_B2("b2_0");
        a1_main.add();
        a2_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a2_main.b1_0.add();
        a2_main.b2_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a2_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum);

        int result = a1_main.sum + a2_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1815)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00150_A1 {
        Nocycle_a_00150_B1 b1_0;
        Nocycle_a_00150_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00150_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00150_A2 {
        Nocycle_a_00150_B1 b1_0;
        Nocycle_a_00150_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00150_A2(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00150_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00150_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00150_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00150_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00160B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00160_A1 a1_main = new Nocycle_a_00160_A1("a1_main");
        Nocycle_a_00160_A2 a2_main = new Nocycle_a_00160_A2("a2_main");
        Nocycle_a_00160_A3 a3_main = new Nocycle_a_00160_A3("a3_main");
        Nocycle_a_00160_A4 a4_main = new Nocycle_a_00160_A4("a4_main");
        a1_main.b1_0 = new Nocycle_a_00160_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00160_B2("b2_0");
        a2_main.b1_0 = new Nocycle_a_00160_B1("b1_0");
        a2_main.b2_0 = new Nocycle_a_00160_B2("b2_0");
        a3_main.b1_0 = new Nocycle_a_00160_B1("b1_0");
        a3_main.b2_0 = new Nocycle_a_00160_B2("b2_0");
        a4_main.b1_0 = new Nocycle_a_00160_B1("b1_0");
        a4_main.b2_0 = new Nocycle_a_00160_B2("b2_0");
        a1_main.add();
        a2_main.add();
        a3_main.add();
        a4_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a2_main.b1_0.add();
        a2_main.b2_0.add();
        a3_main.b1_0.add();
        a3_main.b2_0.add();
        a4_main.b1_0.add();
        a4_main.b2_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a2_main.sum+a3_main.sum+a4_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum);

        int result = a1_main.sum + a2_main.sum + a3_main.sum + a4_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 2828)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00160_A1 {
        Nocycle_a_00160_B1 b1_0;
        Nocycle_a_00160_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00160_A1(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00160_A2 {
        Nocycle_a_00160_B1 b1_0;
        Nocycle_a_00160_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00160_A2(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00160_A3 {
        Nocycle_a_00160_B1 b1_0;
        Nocycle_a_00160_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00160_A3(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00160_A4 {
        Nocycle_a_00160_B1 b1_0;
        Nocycle_a_00160_B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00160_A4(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a;
        }
    }

    class Nocycle_a_00160_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00160_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00160_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00160_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

class ThreadRc_00170B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_00170_A1 a1_main = new Nocycle_a_00170_A1("a1_main");
        Nocycle_a_00170_A2 a2_main = new Nocycle_a_00170_A2("a2_main");
        Nocycle_a_00170_A3 a3_main = new Nocycle_a_00170_A3("a3_main");
        Nocycle_a_00170_A4 a4_main = new Nocycle_a_00170_A4("a4_main");
        a1_main.b1_0 = new Nocycle_a_00170_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_00170_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_00170_B3("b3_0");
        a2_main.b1_0 = new Nocycle_a_00170_B1("b1_0");
        a2_main.b2_0 = new Nocycle_a_00170_B2("b2_0");
        a2_main.b3_0 = new Nocycle_a_00170_B3("b3_0");
        a3_main.b1_0 = new Nocycle_a_00170_B1("b1_0");
        a3_main.b2_0 = new Nocycle_a_00170_B2("b2_0");
        a3_main.b3_0 = new Nocycle_a_00170_B3("b3_0");
        a4_main.b1_0 = new Nocycle_a_00170_B1("b1_0");
        a4_main.b2_0 = new Nocycle_a_00170_B2("b2_0");
        a4_main.b3_0 = new Nocycle_a_00170_B3("b3_0");
        a1_main.add();
        a2_main.add();
        a3_main.add();
        a4_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a2_main.b1_0.add();
        a2_main.b2_0.add();
        a2_main.b3_0.add();
        a3_main.b1_0.add();
        a3_main.b2_0.add();
        a3_main.b3_0.add();
        a4_main.b1_0.add();
        a4_main.b2_0.add();
        a4_main.b3_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a2_main.sum+a3_main.sum+a4_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum);

        int result = a1_main.sum + a2_main.sum + a3_main.sum + a4_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 4046)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_00170_A1 {
        Nocycle_a_00170_B1 b1_0;
        Nocycle_a_00170_B2 b2_0;
        Nocycle_a_00170_B3 b3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00170_A1(String strObjectName) {
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

    class Nocycle_a_00170_A2 {
        Nocycle_a_00170_B1 b1_0;
        Nocycle_a_00170_B2 b2_0;
        Nocycle_a_00170_B3 b3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00170_A2(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            b3_0 = null;
            a = 102;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a + b3_0.a;
        }
    }

    class Nocycle_a_00170_A3 {
        Nocycle_a_00170_B1 b1_0;
        Nocycle_a_00170_B2 b2_0;
        Nocycle_a_00170_B3 b3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00170_A3(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            b3_0 = null;
            a = 103;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a + b3_0.a;
        }
    }

    class Nocycle_a_00170_A4 {
        Nocycle_a_00170_B1 b1_0;
        Nocycle_a_00170_B2 b2_0;
        Nocycle_a_00170_B3 b3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00170_A4(String strObjectName) {
            b1_0 = null;
            b2_0 = null;
            b3_0 = null;
            a = 104;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a + b2_0.a + b3_0.a;
        }
    }

    class Nocycle_a_00170_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00170_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00170_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00170_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_00170_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_00170_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}


class ThreadRc_00180B extends Thread {
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
        a2_main.b2_0.d1_0 = new Nocycle_a_00180_D1("d1_0");
        a2_main.b2_0.d2_0 = new Nocycle_a_00180_D2("d2_0");
        a2_main.b2_0.d3_0 = new Nocycle_a_00180_D3("d3_0");
        a2_main.b2_0.c1_0.d1_0 = new Nocycle_a_00180_D1("d1_0");

        a3_main.b2_0 = new Nocycle_a_00180_B2("b2_0");
        a3_main.b2_0.c1_0 = new Nocycle_a_00180_C1("c1_0");
        a3_main.b2_0.c1_0.d1_0 = new Nocycle_a_00180_D1("d1_0");
        a3_main.b2_0.d1_0 = new Nocycle_a_00180_D1("d1_0");
        a3_main.b2_0.d2_0 = new Nocycle_a_00180_D2("d2_0");
        a3_main.b2_0.d3_0 = new Nocycle_a_00180_D3("d3_0");

        a3_main.c2_0 = new Nocycle_a_00180_C2("c2_0");
        a3_main.c2_0.d2_0 = new Nocycle_a_00180_D2("d2_0");
        a3_main.c2_0.d3_0 = new Nocycle_a_00180_D3("d3_0");

        a4_main.b3_0 = new Nocycle_a_00180_B3("b3_0");
        a4_main.b3_0.c1_0 = new Nocycle_a_00180_C1("c1_0");
        a4_main.b3_0.c1_0.d1_0 = new Nocycle_a_00180_D1("d1_0");
        a4_main.c2_0 = new Nocycle_a_00180_C2("c2_0");
        a4_main.c2_0.d2_0 = new Nocycle_a_00180_D2("d2_0");
        a4_main.c2_0.d3_0 = new Nocycle_a_00180_D3("d3_0");

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

//         System.out.printf("a1_main.sum=%d\n",a1_main.sum);
//         System.out.printf("a2_main.sum=%d\n",a2_main.sum);
//         System.out.printf("a3_main.sum=%d\n",a3_main.sum);
//         System.out.printf("a4_main.sum=%d\n",a4_main.sum);
//         System.out.printf("a1_main.b1_0.sum=%d\n",a1_main.b1_0.sum);
//         System.out.printf("a2_main.b2_0.sum=%d\n",a2_main.b2_0.sum);
//         System.out.printf("a4_main.b3_0.sum=%d\n",a4_main.b3_0.sum);
//         System.out.printf("a2_main.b2_0.c1_0.sum=%d\n",a2_main.b2_0.c1_0.sum);
//         System.out.printf("a3_main.c2_0.sum=%d\n",a3_main.c2_0.sum);
//         System.out.printf("a1_main.d1_0.sum=%d\n",a1_main.d1_0.sum);
//         System.out.printf("a3_main.c2_0.d2_0.sum=%d\n",a3_main.c2_0.d2_0.sum);
//         System.out.printf("a3_main.c2_0.d3_0.sum=%d\n",a3_main.c2_0.d3_0.sum);


//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a2_main.sum+a3_main.sum+a4_main.sum+a1_main.b1_0.sum+a2_main.b2_0.sum+a4_main.b3_0.sum+a2_main.b2_0.c1_0.sum+a3_main.c2_0.sum+a1_main.d1_0.sum+a3_main.c2_0.d2_0.sum+a3_main.c2_0.d3_0.sum);

        int result = a1_main.sum + a2_main.sum + a3_main.sum + a4_main.sum + a1_main.b1_0.sum + a2_main.b2_0.sum + a4_main.b3_0.sum + a2_main.b2_0.c1_0.sum + a3_main.c2_0.sum + a1_main.d1_0.sum + a3_main.c2_0.d2_0.sum + a3_main.c2_0.d3_0.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 9260)
            checkout = true;
        //System.out.println(checkout);
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
        Nocycle_a_00180_D1 d1_0;
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
        Nocycle_a_00180_D2 d2_0;
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

public class Nocycle_am_00190 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_00010B A1_00010 = new ThreadRc_00010B();
        ThreadRc_00010B A2_00010 = new ThreadRc_00010B();
        ThreadRc_00020B A1_00020 = new ThreadRc_00020B();
        ThreadRc_00020B A2_00020 = new ThreadRc_00020B();
        ThreadRc_00030B A1_00030 = new ThreadRc_00030B();
        ThreadRc_00030B A2_00030 = new ThreadRc_00030B();
        ThreadRc_00040B A1_00040 = new ThreadRc_00040B();
        ThreadRc_00040B A2_00040 = new ThreadRc_00040B();
        ThreadRc_00050B A1_00050 = new ThreadRc_00050B();
        ThreadRc_00050B A2_00050 = new ThreadRc_00050B();
        ThreadRc_00060B A1_00060 = new ThreadRc_00060B();
        ThreadRc_00060B A2_00060 = new ThreadRc_00060B();
        ThreadRc_00070B A1_00070 = new ThreadRc_00070B();
        ThreadRc_00070B A2_00070 = new ThreadRc_00070B();
        ThreadRc_00080B A1_00080 = new ThreadRc_00080B();
        ThreadRc_00080B A2_00080 = new ThreadRc_00080B();
        ThreadRc_00090B A1_00090 = new ThreadRc_00090B();
        ThreadRc_00090B A2_00090 = new ThreadRc_00090B();
        ThreadRc_00100B A1_00100 = new ThreadRc_00100B();
        ThreadRc_00100B A2_00100 = new ThreadRc_00100B();
        ThreadRc_00110B A1_00110 = new ThreadRc_00110B();
        ThreadRc_00110B A2_00110 = new ThreadRc_00110B();
        ThreadRc_00120B A1_00120 = new ThreadRc_00120B();
        ThreadRc_00120B A2_00120 = new ThreadRc_00120B();
        ThreadRc_00130B A1_00130 = new ThreadRc_00130B();
        ThreadRc_00130B A2_00130 = new ThreadRc_00130B();
        ThreadRc_00140B A1_00140 = new ThreadRc_00140B();
        ThreadRc_00140B A2_00140 = new ThreadRc_00140B();
        ThreadRc_00150B A1_00150 = new ThreadRc_00150B();
        ThreadRc_00150B A2_00150 = new ThreadRc_00150B();
        ThreadRc_00160B A1_00160 = new ThreadRc_00160B();
        ThreadRc_00160B A2_00160 = new ThreadRc_00160B();
        ThreadRc_00170B A1_00170 = new ThreadRc_00170B();
        ThreadRc_00170B A2_00170 = new ThreadRc_00170B();
        ThreadRc_00180B A1_00180 = new ThreadRc_00180B();
        ThreadRc_00180B A2_00180 = new ThreadRc_00180B();
        A1_00010.start();
        A2_00010.start();
        A1_00020.start();
        A2_00020.start();
        A1_00030.start();
        A2_00030.start();
        A1_00040.start();
        A2_00040.start();
        A1_00050.start();
        A2_00050.start();
        A1_00060.start();
        A2_00060.start();
        A1_00070.start();
        A2_00070.start();
        A1_00080.start();
        A2_00080.start();
        A1_00090.start();
        A2_00090.start();
        A1_00100.start();
        A2_00100.start();
        A1_00110.start();
        A2_00110.start();
        A1_00120.start();
        A2_00120.start();
        A1_00130.start();
        A2_00130.start();
        A1_00140.start();
        A2_00140.start();
        A1_00150.start();
        A2_00150.start();
        A1_00160.start();
        A2_00160.start();
        A1_00170.start();
        A2_00170.start();
        A1_00180.start();
        A2_00180.start();
        try {
            A1_00010.join();
            A2_00010.join();
            A1_00020.join();
            A2_00020.join();
            A1_00030.join();
            A2_00030.join();
            A1_00040.join();
            A2_00040.join();
            A1_00050.join();
            A2_00050.join();
            A1_00060.join();
            A2_00060.join();
            A1_00070.join();
            A2_00070.join();
            A1_00080.join();
            A2_00080.join();
            A1_00090.join();
            A2_00090.join();
            A1_00100.join();
            A2_00100.join();
            A1_00110.join();
            A2_00110.join();
            A1_00120.join();
            A2_00120.join();
            A1_00130.join();
            A2_00130.join();
            A1_00140.join();
            A2_00140.join();
            A1_00150.join();
            A2_00150.join();
            A1_00160.join();
            A2_00160.join();
            A1_00170.join();
            A2_00170.join();
            A1_00180.join();
            A2_00180.join();
        } catch (InterruptedException e) {
        }
        if (A1_00010.check() && A2_00010.check() && A1_00020.check() && A2_00020.check() && A1_00030.check() && A2_00030.check() && A1_00040.check() && A2_00040.check() && A1_00050.check() && A2_00050.check() && A1_00060.check() && A2_00060.check() && A1_00070.check() && A2_00070.check() && A1_00080.check() && A2_00080.check() && A1_00090.check() && A2_00090.check() && A1_00100.check() && A2_00100.check() && A1_00110.check() && A2_00110.check() && A1_00120.check() && A2_00120.check() && A1_00130.check() && A2_00130.check() && A1_00140.check() && A2_00140.check() && A1_00150.check() && A2_00150.check() && A1_00160.check() && A2_00160.check() && A1_00170.check() && A2_00170.check() && A1_00180.check() && A2_00180.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n