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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread02/RCAnnotationThread03.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Multi Thread reads or writes static para.mofidfy from Cycle_B_2_00130
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RCAnnotationThread03.java
 *- @ExecuteClass: RCAnnotationThread03
 *- @ExecuteArgs:

 *
 */

import com.huawei.ark.annotation.Weak;
import com.huawei.ark.annotation.Unowned;

class RCAnnotationThread03_1 extends Thread {
    public void run() {
        RCAnnotationThread03 rcth01 = new RCAnnotationThread03();
        try {
            rcth01.setA1null();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }
}

class RCAnnotationThread03_2 extends Thread {
    public void run() {
        RCAnnotationThread03 rcth01 = new RCAnnotationThread03();
        try {
            rcth01.setA4null();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }
}

class RCAnnotationThread03_3 extends Thread {
    public void run() {
        RCAnnotationThread03 rcth01 = new RCAnnotationThread03();
        try {
            rcth01.setA5null();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }

    }
}

public class RCAnnotationThread03 {
    private volatile static RCAnnotationThread03_A1 a1_main = null;
    private volatile static RCAnnotationThread03_A5 a5_main = null;

    RCAnnotationThread03() {
        synchronized (this) {
            try {
                a1_main = new RCAnnotationThread03_A1();
                a1_main.a2_0 = new RCAnnotationThread03_A2();
                a1_main.a2_0.a3_0 = new RCAnnotationThread03_A3();
                a1_main.a2_0.a3_0.a4_0 = new RCAnnotationThread03_A4();
                a1_main.a2_0.a3_0.a4_0.a1_0 = a1_main;
                a5_main = new RCAnnotationThread03_A5();
                a1_main.a2_0.a3_0.a4_0.a6_0 = new RCAnnotationThread03_A6();
                a1_main.a2_0.a3_0.a4_0.a6_0.a5_0 = a5_main;
                a5_main.a3_0 = a1_main.a2_0.a3_0;
            } catch (NullPointerException e) {
            }
        }
    }

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        System.out.println("ExpectResult");
    }


    private static void rc_testcase_main_wrapper() {
        RCAnnotationThread03_1 t1 = new RCAnnotationThread03_1();
        RCAnnotationThread03_2 t2 = new RCAnnotationThread03_2();
        RCAnnotationThread03_3 t3 = new RCAnnotationThread03_3();
        t1.start();
        t2.start();
        t3.start();
        try {
            t1.join();
            t2.join();
            t3.join();
        } catch (InterruptedException e) {
        }
    }

    public void setA1null() {
        a1_main = null;
    }

    public void setA4null() {
        try {
            a1_main.a2_0.a3_0.a4_0 = null;
            a5_main.a3_0.a4_0 = null;
        } catch (NullPointerException e) {
        }
    }

    public void setA5null() {
        a5_main = null;
    }

    static class RCAnnotationThread03_A1 {
        volatile RCAnnotationThread03_A2 a2_0;
        volatile RCAnnotationThread03_A4 a4_0;
        int a;
        int sum;

        RCAnnotationThread03_A1() {
            a2_0 = null;
            a4_0 = null;
            a = 1;
            sum = 0;
        }

        void add() {
            sum = a + a2_0.a;
        }
    }


    static class RCAnnotationThread03_A2 {
        volatile RCAnnotationThread03_A3 a3_0;
        int a;
        int sum;

        RCAnnotationThread03_A2() {
            a3_0 = null;
            a = 2;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }


    static class RCAnnotationThread03_A3 {
        volatile RCAnnotationThread03_A4 a4_0;
        int a;
        int sum;

        RCAnnotationThread03_A3() {
            a4_0 = null;
            a = 3;
            sum = 0;
        }

        void add() {
            sum = a + a4_0.a;
        }
    }


    static class RCAnnotationThread03_A4 {
        @Weak
        volatile RCAnnotationThread03_A1 a1_0;
        volatile RCAnnotationThread03_A6 a6_0;
        int a;
        int sum;

        RCAnnotationThread03_A4() {
            a1_0 = null;
            a6_0 = null;
            a = 4;
            sum = 0;
        }

        void add() {
            sum = a + a1_0.a + a6_0.a;
        }
    }

    static class RCAnnotationThread03_A5 {
        @Unowned
        volatile RCAnnotationThread03_A3 a3_0;
        int a;
        int sum;

        RCAnnotationThread03_A5() {
            a3_0 = null;
            a = 5;
            sum = 0;
        }

        void add() {
            sum = a + a3_0.a;
        }
    }

    static class RCAnnotationThread03_A6 {
        @Unowned
        volatile RCAnnotationThread03_A5 a5_0;
        int a;
        int sum;

        RCAnnotationThread03_A6() {
            a5_0 = null;
            a = 6;
            sum = 0;
        }

        void add() {
            sum = a + a5_0.a;
        }
    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n