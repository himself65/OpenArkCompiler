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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Cycle_Bm_1_00210.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:make Cycle_B_1_xx together to this Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_Bm_1_00210.java
 *- @ExecuteClass: Cycle_Bm_1_00210
 *- @ExecuteArgs:
 */
class ThreadRc_Cycle_Bm_1_00010B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00010_A1 a1_0 = new Cycle_B_1_00010_A1();
        a1_0.a1_0 = a1_0;
        a1_0.add();
        int nsum = a1_0.sum;
        //System.out.println(nsum);

        if (nsum == 246)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00010_A1 {
        Cycle_B_1_00010_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00010_A1() {
            a1_0 = null;
            a = 123;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00020B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00020_A1 a1_0 = new Cycle_B_1_00020_A1();
        a1_0.a2_0 = new Cycle_B_1_00020_A2();
        a1_0.a2_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();

        int nsum = (a1_0.sum + a1_0.a2_0.sum);
        //System.out.println(nsum);

        if (nsum == 6)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00020_A1 {
        Cycle_B_1_00020_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00020_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00020_A2 {
        Cycle_B_1_00020_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00020_A2() {
            a1_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00030B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00030_A1 a1_0 = new Cycle_B_1_00030_A1();
        a1_0.a2_0 = new Cycle_B_1_00030_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00030_A3();
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum);
        //System.out.println(nsum);

        if (nsum == 12)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00030_A1 {
        Cycle_B_1_00030_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00030_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00030_A2 {
        Cycle_B_1_00030_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00030_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00030_A3 {
        Cycle_B_1_00030_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00030_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00040B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00040_A1 a1_0 = new Cycle_B_1_00040_A1();
        a1_0.a2_0 = new Cycle_B_1_00040_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00040_A3();
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.a2_0.a3_0.a4_0 = new Cycle_B_1_00040_A4();
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a1_0.a2_0.a3_0.a4_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a1_0.a2_0.a3_0.a4_0.sum;
        //System.out.println(nsum);

        if (nsum == 21)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00040_A1 {
        Cycle_B_1_00040_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00040_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00040_A2 {
        Cycle_B_1_00040_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00040_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00040_A3 {
        Cycle_B_1_00040_A1 a1_0;
        Cycle_B_1_00040_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00040_A3() {
            a1_0 = null;
            a4_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00040_A4 {

        int a;
        int sum;

        Cycle_B_1_00040_A4() {
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + 5;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00050B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00050_A1 a1_0 = new Cycle_B_1_00050_A1();
        a1_0.a2_0 = new Cycle_B_1_00050_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00050_A3();
        a1_0.a2_0.a4_0 = new Cycle_B_1_00050_A4();
        a1_0.a2_0.a5_0 = new Cycle_B_1_00050_A5();
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a1_0.a2_0.a4_0.add();
        a1_0.a2_0.a5_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a1_0.a2_0.a4_0.sum + a1_0.a2_0.a5_0.sum;
        //System.out.println(nsum);

        if (nsum == 34)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00050_A1 {
        Cycle_B_1_00050_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00050_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00050_A2 {
        Cycle_B_1_00050_A3 a3_0;
        Cycle_B_1_00050_A4 a4_0;
        Cycle_B_1_00050_A5 a5_0;
        int a;
        int sum;

        Cycle_B_1_00050_A2() {
            a3_0 = null;
            a4_0 = null;
            a5_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00050_A3 {
        Cycle_B_1_00050_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00050_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00050_A4 {

        int a;
        int sum;

        Cycle_B_1_00050_A4() {
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + 5;
        }
    }

    class Cycle_B_1_00050_A5 {

        int a;
        int sum;

        Cycle_B_1_00050_A5() {
            a = 6;
            sum = 0;
        }

        void add() {
            sum = a + 7;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00060B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00060_A1 a1_0 = new Cycle_B_1_00060_A1();
        a1_0.a2_0 = new Cycle_B_1_00060_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00060_A3();
        Cycle_B_1_00060_A4 a4_0 = new Cycle_B_1_00060_A4();
        a4_0.a3_0 = a1_0.a2_0.a3_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum;
        //System.out.println(nsum);

        if (nsum == 19)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00060_A1 {
        Cycle_B_1_00060_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00060_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00060_A2 {
        Cycle_B_1_00060_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00060_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00060_A3 {
        Cycle_B_1_00060_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00060_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00060_A4 {
        Cycle_B_1_00060_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00060_A4() {
            a3_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00070B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00070_A1 a1_0 = new Cycle_B_1_00070_A1();
        a1_0.a2_0 = new Cycle_B_1_00070_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00070_A3();
        a1_0.a2_0.a3_0.a4_0 = new Cycle_B_1_00070_A4();

        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a1_0.a2_0.a3_0.a4_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a1_0.a2_0.a3_0.a4_0.sum;
        //System.out.println(nsum);

        if (nsum == 21)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00070_A1 {
        Cycle_B_1_00070_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00070_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00070_A2 {
        Cycle_B_1_00070_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00070_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00070_A3 {
        Cycle_B_1_00070_A1 a1_0;
        Cycle_B_1_00070_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00070_A3() {
            a1_0 = null;
            a4_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00070_A4 {
        int a;
        int sum;

        Cycle_B_1_00070_A4() {
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + 5;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00080B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00080_A1 a1_0 = new Cycle_B_1_00080_A1();
        a1_0.a2_0 = new Cycle_B_1_00080_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00080_A3();
        a1_0.a2_0.a3_0.a4_0 = new Cycle_B_1_00080_A4();
        a1_0.a2_0.a3_0.a5_0 = new Cycle_B_1_00080_A5();
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a1_0.a2_0.a3_0.a4_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a1_0.a2_0.a3_0.a4_0.sum + a1_0.a2_0.a3_0.a5_0.sum;
        //System.out.println(nsum);

        if (nsum == 56)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00080_A1 {
        Cycle_B_1_00080_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00080_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00080_A2 {
        Cycle_B_1_00080_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00080_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00080_A3 {
        Cycle_B_1_00080_A1 a1_0;
        Cycle_B_1_00080_A4 a4_0;
        Cycle_B_1_00080_A5 a5_0;
        int a;
        int sum;

        Cycle_B_1_00080_A3() {
            a1_0 = null;
            a4_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            a4_0.add();
            a5_0.add();
            sum = a + a1_0.a + a4_0.sum + a5_0.sum;
        }
    }

    class Cycle_B_1_00080_A4 {
        int a;
        int sum;

        Cycle_B_1_00080_A4() {
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + 5;
        }
    }

    class Cycle_B_1_00080_A5 {
        int a;
        int sum;

        Cycle_B_1_00080_A5() {
            a = 6;
            sum = 0;
        }

        void add() {
            sum = a + 7;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00090B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00090_A1 a1_0 = new Cycle_B_1_00090_A1();
        a1_0.a2_0 = new Cycle_B_1_00090_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00090_A3();
        Cycle_B_1_00090_A4 a4_0 = new Cycle_B_1_00090_A4();
        Cycle_B_1_00090_A5 a5_0 = new Cycle_B_1_00090_A5();
        a4_0.a2_0 = a1_0.a2_0;
        a5_0.a2_0 = a1_0.a2_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        a5_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum + a5_0.sum;
        //System.out.println(nsum);

        if (nsum == 25)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00090_A1 {
        Cycle_B_1_00090_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00090_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00090_A2 {
        Cycle_B_1_00090_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00090_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00090_A3 {
        Cycle_B_1_00090_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00090_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00090_A4 {
        Cycle_B_1_00090_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00090_A4() {
            a2_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00090_A5 {
        Cycle_B_1_00090_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00090_A5() {
            a2_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00100B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00100_A1 a1_0 = new Cycle_B_1_00100_A1();
        a1_0.a2_0 = new Cycle_B_1_00100_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00100_A3();
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.a2_0.a4_0 = new Cycle_B_1_00100_A4();
        a1_0.a2_0.a4_0.a5_0 = new Cycle_B_1_00100_A5();
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a1_0.a2_0.a4_0.add();
        a1_0.a2_0.a4_0.a5_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a1_0.a2_0.a4_0.sum + a1_0.a2_0.a4_0.a5_0.sum;
        //System.out.println(nsum);

        if (nsum == 38)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00100_A1 {
        Cycle_B_1_00100_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00100_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00100_A2 {
        Cycle_B_1_00100_A3 a3_0;
        Cycle_B_1_00100_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00100_A2() {
            a3_0 = null;
            a4_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00100_A3 {
        Cycle_B_1_00100_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00100_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00100_A4 {

        int a;
        int sum;
        Cycle_B_1_00100_A5 a5_0;

        Cycle_B_1_00100_A4() {
            a5_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            a5_0.add();
            sum = a + a5_0.sum;
        }
    }

    class Cycle_B_1_00100_A5 {

        int a;
        int sum;

        Cycle_B_1_00100_A5() {
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + 6;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00110B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00110_A1 a1_0 = new Cycle_B_1_00110_A1();
        a1_0.a2_0 = new Cycle_B_1_00110_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00110_A3();
        Cycle_B_1_00110_A5 a5_0 = new Cycle_B_1_00110_A5();
        a5_0.a4_0 = new Cycle_B_1_00110_A4();
        a5_0.a4_0.a2_0 = a1_0.a2_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a5_0.a4_0.add();
        a5_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a5_0.a4_0.sum + a5_0.sum;
        //System.out.println(nsum);

        if (nsum == 27)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00110_A1 {
        Cycle_B_1_00110_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00110_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00110_A2 {
        Cycle_B_1_00110_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00110_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00110_A3 {
        Cycle_B_1_00110_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00110_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00110_A4 {
        Cycle_B_1_00110_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00110_A4() {
            a2_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00110_A5 {
        Cycle_B_1_00110_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00110_A5() {
            a4_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a4_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00120B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00120_A1 a1_0 = new Cycle_B_1_00120_A1();
        a1_0.a2_0 = new Cycle_B_1_00120_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00120_A3();
        Cycle_B_1_00120_A5 a5_0 = new Cycle_B_1_00120_A5();
        a5_0.a4_0 = new Cycle_B_1_00120_A4();
        a5_0.a4_0.a3_0 = a1_0.a2_0.a3_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a5_0.a4_0.add();
        a5_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a5_0.a4_0.sum + a5_0.sum;
        //System.out.println(nsum);

        if (nsum == 28)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00120_A1 {
        Cycle_B_1_00120_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00120_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00120_A2 {
        Cycle_B_1_00120_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00120_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00120_A3 {
        Cycle_B_1_00120_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00120_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00120_A4 {
        Cycle_B_1_00120_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00120_A4() {
            a3_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00120_A5 {
        Cycle_B_1_00120_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00120_A5() {
            a4_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a4_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00130B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00130_A1 a1_0 = new Cycle_B_1_00130_A1();
        Cycle_B_1_00130_A2 a2_0 = new Cycle_B_1_00130_A2();
        Cycle_B_1_00130_A3 a3_0 = new Cycle_B_1_00130_A3();
        a1_0.a3_0 = a3_0;
        a2_0.a3_0 = a3_0;
        a3_0.a1_0 = a1_0;
        a1_0.add();
        a2_0.add();
        a3_0.add();
        int nsum = (a1_0.sum + a2_0.sum + a3_0.sum);
        //System.out.println(nsum);

        if (nsum == 13)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00130_A1 {
        Cycle_B_1_00130_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00130_A1() {
            a3_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00130_A2 {
        Cycle_B_1_00130_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00130_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00130_A3 {
        Cycle_B_1_00130_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00130_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00140B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00140_A1 a1_0 = new Cycle_B_1_00140_A1();
        a1_0.a2_0 = new Cycle_B_1_00140_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00140_A3();
        Cycle_B_1_00140_A4 a4_0 = new Cycle_B_1_00140_A4();
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a4_0.a3_0 = a1_0.a2_0.a3_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum);
        //System.out.println(nsum);

        if (nsum == 12)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00140_A1 {
        Cycle_B_1_00140_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00140_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00140_A2 {
        Cycle_B_1_00140_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00140_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00140_A3 {
        Cycle_B_1_00140_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00140_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00140_A4 {
        Cycle_B_1_00140_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00140_A4() {
            a3_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00150B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00150_A1 a1_0 = new Cycle_B_1_00150_A1();
        a1_0.a2_0 = new Cycle_B_1_00150_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00150_A3();
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.a2_0.a3_0.a4_0 = new Cycle_B_1_00150_A4();
        a1_0.a2_0.a3_0.a4_0.a5_0 = new Cycle_B_1_00150_A5();
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a1_0.a2_0.a3_0.a4_0.add();
        a1_0.a2_0.a3_0.a4_0.a5_0.add();
        int nsum = a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a1_0.a2_0.a3_0.a4_0.sum + a1_0.a2_0.a3_0.a4_0.a5_0.sum;
        //System.out.println(nsum);

        if (nsum == 38)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00150_A1 {
        Cycle_B_1_00150_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00150_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00150_A2 {
        Cycle_B_1_00150_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00150_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00150_A3 {
        Cycle_B_1_00150_A1 a1_0;
        Cycle_B_1_00150_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00150_A3() {
            a1_0 = null;
            a4_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00150_A4 {

        int a;
        int sum;
        Cycle_B_1_00150_A5 a5_0;

        Cycle_B_1_00150_A4() {
            a5_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            a5_0.add();
            sum = a + a5_0.sum;
        }
    }

    class Cycle_B_1_00150_A5 {

        int a;
        int sum;

        Cycle_B_1_00150_A5() {
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + 6;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00160B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00160_A1 a1_0 = new Cycle_B_1_00160_A1();
        a1_0.a2_0 = new Cycle_B_1_00160_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00160_A3();
        Cycle_B_1_00160_A4 a4_0 = new Cycle_B_1_00160_A4();
        a4_0.a1_0 = a1_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum);
        //System.out.println(nsum);

        if (nsum == 17)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00160_A1 {
        Cycle_B_1_00160_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00160_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00160_A2 {
        Cycle_B_1_00160_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00160_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00160_A3 {
        Cycle_B_1_00160_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00160_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00160_A4 {
        Cycle_B_1_00160_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00160_A4() {
            a1_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00170B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00170_A1 a1_0 = new Cycle_B_1_00170_A1();
        a1_0.a2_0 = new Cycle_B_1_00170_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00170_A3();
        Cycle_B_1_00170_A4 a4_0 = new Cycle_B_1_00170_A4();
        Cycle_B_1_00170_A5 a5_0 = new Cycle_B_1_00170_A5();
        a5_0.a4_0 = a4_0;
        a4_0.a1_0 = a1_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        a5_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum + a5_0.sum);
        //System.out.println(nsum);

        if (nsum == 26)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00170_A1 {
        Cycle_B_1_00170_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00170_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00170_A2 {
        Cycle_B_1_00170_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00170_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00170_A3 {
        Cycle_B_1_00170_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00170_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00170_A4 {
        Cycle_B_1_00170_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00170_A4() {
            a1_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00170_A5 {
        Cycle_B_1_00170_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00170_A5() {
            a4_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a4_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00180B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00180_A1 a1_0 = new Cycle_B_1_00180_A1();
        a1_0.a2_0 = new Cycle_B_1_00180_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00180_A3();
        Cycle_B_1_00180_A4 a4_0 = new Cycle_B_1_00180_A4();
        Cycle_B_1_00180_A5 a5_0 = new Cycle_B_1_00180_A5();
        a4_0.a1_0 = a1_0;
        a5_0.a1_0 = a1_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        a5_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum + a5_0.sum);
        //System.out.println(nsum);

        if (nsum == 23)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00180_A1 {
        Cycle_B_1_00180_A2 a2_0;
        int a;
        int sum;

        Cycle_B_1_00180_A1() {
            a2_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00180_A2 {
        Cycle_B_1_00180_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00180_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00180_A3 {
        Cycle_B_1_00180_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00180_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00180_A4 {
        Cycle_B_1_00180_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00180_A4() {
            a1_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00180_A5 {
        Cycle_B_1_00180_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00180_A5() {
            a1_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00190B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00190_A1 a1_0 = new Cycle_B_1_00190_A1();
        a1_0.a2_0 = new Cycle_B_1_00190_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00190_A3();
        Cycle_B_1_00190_A4 a4_0 = new Cycle_B_1_00190_A4();
        a1_0.a4_0 = a4_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum);
        //System.out.println(nsum);

        if (nsum == 21)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00190_A1 {
        Cycle_B_1_00190_A2 a2_0;
        Cycle_B_1_00190_A4 a4_0;
        int a;
        int sum;

        Cycle_B_1_00190_A1() {
            a2_0 = null;
            a4_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00190_A2 {
        Cycle_B_1_00190_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00190_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00190_A3 {
        Cycle_B_1_00190_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00190_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00190_A4 {
        int a;
        int sum;

        Cycle_B_1_00190_A4() {
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + 5;
        }
    }
}

class ThreadRc_Cycle_Bm_1_00200B extends Thread {
    private boolean checkout;

    public void run() {
        Cycle_B_1_00200_A1 a1_0 = new Cycle_B_1_00200_A1();
        a1_0.a2_0 = new Cycle_B_1_00200_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_1_00200_A3();
        Cycle_B_1_00200_A4 a4_0 = new Cycle_B_1_00200_A4();
        Cycle_B_1_00200_A5 a5_0 = new Cycle_B_1_00200_A5();
        a1_0.a4_0 = a4_0;
        a1_0.a5_0 = a5_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        a5_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum + a5_0.sum);
        //System.out.println(nsum);

        if (nsum == 38)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Cycle_B_1_00200_A1 {
        Cycle_B_1_00200_A2 a2_0;
        Cycle_B_1_00200_A4 a4_0;
        Cycle_B_1_00200_A5 a5_0;
        int a;
        int sum;

        Cycle_B_1_00200_A1() {
            a2_0 = null;
            a4_0 = null;
            a5_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }

    class Cycle_B_1_00200_A2 {
        Cycle_B_1_00200_A3 a3_0;
        int a;
        int sum;

        Cycle_B_1_00200_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    class Cycle_B_1_00200_A3 {
        Cycle_B_1_00200_A1 a1_0;
        int a;
        int sum;

        Cycle_B_1_00200_A3() {
            a1_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a;
        }
    }

    class Cycle_B_1_00200_A4 {
        int a;
        int sum;

        Cycle_B_1_00200_A4() {
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + 6;
        }
    }

    class Cycle_B_1_00200_A5 {
        int a;
        int sum;

        Cycle_B_1_00200_A5() {
            a = 7;
            sum = 0;
        }

        void add() {
            sum = a + 8;
        }
    }
}

public class Cycle_Bm_1_00210 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Cycle_Bm_1_00010B A1_00010 = new ThreadRc_Cycle_Bm_1_00010B();
        ThreadRc_Cycle_Bm_1_00010B A2_00010 = new ThreadRc_Cycle_Bm_1_00010B();
        ThreadRc_Cycle_Bm_1_00020B A1_00020 = new ThreadRc_Cycle_Bm_1_00020B();
        ThreadRc_Cycle_Bm_1_00020B A2_00020 = new ThreadRc_Cycle_Bm_1_00020B();
        ThreadRc_Cycle_Bm_1_00030B A1_00030 = new ThreadRc_Cycle_Bm_1_00030B();
        ThreadRc_Cycle_Bm_1_00030B A2_00030 = new ThreadRc_Cycle_Bm_1_00030B();
        ThreadRc_Cycle_Bm_1_00040B A1_00040 = new ThreadRc_Cycle_Bm_1_00040B();
        ThreadRc_Cycle_Bm_1_00040B A2_00040 = new ThreadRc_Cycle_Bm_1_00040B();
        ThreadRc_Cycle_Bm_1_00050B A1_00050 = new ThreadRc_Cycle_Bm_1_00050B();
        ThreadRc_Cycle_Bm_1_00050B A2_00050 = new ThreadRc_Cycle_Bm_1_00050B();
        ThreadRc_Cycle_Bm_1_00060B A1_00060 = new ThreadRc_Cycle_Bm_1_00060B();
        ThreadRc_Cycle_Bm_1_00060B A2_00060 = new ThreadRc_Cycle_Bm_1_00060B();
        ThreadRc_Cycle_Bm_1_00070B A1_00070 = new ThreadRc_Cycle_Bm_1_00070B();
        ThreadRc_Cycle_Bm_1_00070B A2_00070 = new ThreadRc_Cycle_Bm_1_00070B();
        ThreadRc_Cycle_Bm_1_00080B A1_00080 = new ThreadRc_Cycle_Bm_1_00080B();
        ThreadRc_Cycle_Bm_1_00080B A2_00080 = new ThreadRc_Cycle_Bm_1_00080B();
        ThreadRc_Cycle_Bm_1_00090B A1_00090 = new ThreadRc_Cycle_Bm_1_00090B();
        ThreadRc_Cycle_Bm_1_00090B A2_00090 = new ThreadRc_Cycle_Bm_1_00090B();
        ThreadRc_Cycle_Bm_1_00100B A1_00100 = new ThreadRc_Cycle_Bm_1_00100B();
        ThreadRc_Cycle_Bm_1_00100B A2_00100 = new ThreadRc_Cycle_Bm_1_00100B();
        ThreadRc_Cycle_Bm_1_00110B A1_00110 = new ThreadRc_Cycle_Bm_1_00110B();
        ThreadRc_Cycle_Bm_1_00110B A2_00110 = new ThreadRc_Cycle_Bm_1_00110B();
        ThreadRc_Cycle_Bm_1_00120B A1_00120 = new ThreadRc_Cycle_Bm_1_00120B();
        ThreadRc_Cycle_Bm_1_00120B A2_00120 = new ThreadRc_Cycle_Bm_1_00120B();
        ThreadRc_Cycle_Bm_1_00130B A1_00130 = new ThreadRc_Cycle_Bm_1_00130B();
        ThreadRc_Cycle_Bm_1_00130B A2_00130 = new ThreadRc_Cycle_Bm_1_00130B();
        ThreadRc_Cycle_Bm_1_00140B A1_00140 = new ThreadRc_Cycle_Bm_1_00140B();
        ThreadRc_Cycle_Bm_1_00140B A2_00140 = new ThreadRc_Cycle_Bm_1_00140B();
        ThreadRc_Cycle_Bm_1_00150B A1_00150 = new ThreadRc_Cycle_Bm_1_00150B();
        ThreadRc_Cycle_Bm_1_00150B A2_00150 = new ThreadRc_Cycle_Bm_1_00150B();
        ThreadRc_Cycle_Bm_1_00160B A1_00160 = new ThreadRc_Cycle_Bm_1_00160B();
        ThreadRc_Cycle_Bm_1_00160B A2_00160 = new ThreadRc_Cycle_Bm_1_00160B();
        ThreadRc_Cycle_Bm_1_00170B A1_00170 = new ThreadRc_Cycle_Bm_1_00170B();
        ThreadRc_Cycle_Bm_1_00170B A2_00170 = new ThreadRc_Cycle_Bm_1_00170B();
        ThreadRc_Cycle_Bm_1_00180B A1_00180 = new ThreadRc_Cycle_Bm_1_00180B();
        ThreadRc_Cycle_Bm_1_00180B A2_00180 = new ThreadRc_Cycle_Bm_1_00180B();
        ThreadRc_Cycle_Bm_1_00190B A1_00190 = new ThreadRc_Cycle_Bm_1_00190B();
        ThreadRc_Cycle_Bm_1_00190B A2_00190 = new ThreadRc_Cycle_Bm_1_00190B();
        ThreadRc_Cycle_Bm_1_00200B A1_00200 = new ThreadRc_Cycle_Bm_1_00200B();
        ThreadRc_Cycle_Bm_1_00200B A2_00200 = new ThreadRc_Cycle_Bm_1_00200B();
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
        A1_00190.start();
        A2_00190.start();
        A1_00200.start();
        A2_00200.start();
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
            A1_00190.join();
            A2_00190.join();
            A1_00200.join();
            A2_00200.join();
        } catch (InterruptedException e) {
        }
        if (A1_00010.check() && A2_00010.check() && A1_00020.check() && A2_00020.check() && A1_00030.check() && A2_00030.check() && A1_00040.check() && A2_00040.check() && A1_00050.check() && A2_00050.check() && A1_00060.check() && A2_00060.check() && A1_00070.check() && A2_00070.check() && A1_00080.check() && A2_00080.check() && A1_00090.check() && A2_00090.check() && A1_00100.check() && A2_00100.check() && A1_00110.check() && A2_00110.check() && A1_00120.check() && A2_00120.check() && A1_00130.check() && A2_00130.check() && A1_00140.check() && A2_00140.check() && A1_00150.check() && A2_00150.check() && A1_00160.check() && A2_00160.check() && A1_00170.check() && A2_00170.check() && A1_00180.check() && A2_00180.check() && A1_00190.check() && A2_00190.check() && A1_00200.check() && A2_00200.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n