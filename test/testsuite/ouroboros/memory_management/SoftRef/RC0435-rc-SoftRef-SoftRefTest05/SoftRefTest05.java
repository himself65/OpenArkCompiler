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
 * -@TestCaseID:SoftRefTest05.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: 环上SoftReference对象在正常情况下不会被释放
 *- @Brief:functionTest
 * -#step1：创建一个对象环，由四个对象Class_A、Class_B、Class_C和Class_D组成。这四个对象都一个域，关联着一个SoftReference；
 * -#step2：执行一次后，进行一遍gc，进行环的自学习；
 * -#step3：再执行一次后，判断环关联的软对象不应该被释放。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: SoftRefTest05.java
 *- @ExecuteClass: SoftRefTest05
 *- @ExecuteArgs:
 *
 */

import java.lang.ref.Reference;
import java.lang.ref.SoftReference;

public class SoftRefTest05 {
    static final int TEST_NUM = 1;
    static int check_count = 0;
    static int allSum;

    public static void main(String[] args) {
        for (int i = 0; i < TEST_NUM; i++) {
            test_01();
            Runtime.getRuntime().gc();
            // resultLine = null;
            test_01();

            if (allSum == 20) {
                check_count++;
                System.out.println("sum is srong");
            }

            if (check_count == 0) {
                System.out.println("ExpectResult");
            } else {
                System.out.println(" errorResult ----checkcount" + check_count);
            }

        }
    }

    private static void test_01() {
        Cycle_A a1 = new Cycle_A();
        a1.cyb = new Cycle_B();
        a1.cyb.cyc = new Cycle_C();
        a1.cyb.cyc.cyd = new Cycle_D();
        a1.cyb.cyc.cyd.cya = a1;
        allSum = a1.sum + a1.cyb.sum + a1.cyb.cyc.sum + a1.cyb.cyc.cyd.sum;
        sleep(4000);
        if ((a1.sr.get() == null) || (a1.cyb.sr.get() == null) || (a1.cyb.cyc.pr.get() == null) || (a1.cyb.cyc.cyd.str.get() == null)) {
            check_count++;
        }

    }

    private static void sleep(int sleepnum) {
        try {
            Thread.sleep(sleepnum);
        } catch (InterruptedException e) {
            System.out.println(Thread.currentThread().getStackTrace()[2].getClassName() + " sleep was Interrupted");
        }
    }

}

class Cycle_A {
    static StringBuffer a = new StringBuffer("ref_processor_cycle_A");
    Reference sr;
    Cycle_B cyb;
    int aNum;
    int sum;

    Cycle_A() {
        a = new StringBuffer("ref_processor_cycle_A");
        sr = new SoftReference(a);
        if (sr.get() == null) {
            assert false;
        }
        a = null;
        cyb = null;
        aNum = 1;
    }

    int joinStr() {
        try {
            sum = aNum + cyb.bNum;
        } catch (Exception e) {

        }
        return sum;
    }
}

class Cycle_B {
    static StringBuffer b = new StringBuffer("ref_processor_cycle_B");
    Reference sr;
    Cycle_C cyc;
    int bNum;
    int sum;

    Cycle_B() {
        b = new StringBuffer("ref_processor_cycle_B");
        sr = new SoftReference<>(b);
        if (sr.get() == null) {
            assert false;
        }
        b = null;
        cyc = null;
        bNum = 2;
    }


    int joinStr() {
        try {
            sum = bNum + cyc.cNum;
        } catch (Exception e) {

        }
        return sum;
    }


}

class Cycle_C {
    Reference pr;
    Cycle_D cyd;
    StringBuffer c = new StringBuffer("ref_processor_cycle_C");
    int sum;
    int cNum;

    Cycle_C() {
        c = new StringBuffer("ref_processor_cycle_C");
        pr = new SoftReference<>(c);
        if (pr.get() == null) {
            assert false;
        }
        c = null;
        cyd = null;
        cNum = 3;
    }

    int joinStr() {
        try {
            sum = cNum + cyd.dNum;
        } catch (Exception e) {

        }
        return sum;
    }
}

class Cycle_D {
    Reference str;
    Cycle_A cya;
    StringBuffer d = new StringBuffer("ref_processor_cycle_D");
    int dNum;
    int sum;


    Cycle_D() {
        d = new StringBuffer("ref_processor_cycle_D");
        str = new SoftReference<StringBuffer>(d);
        d = null;
        cya = null;
        dNum = 4;

    }


    int joinStr() {
        try {
            int sum = dNum + cya.aNum;
        } catch (Exception e) {
        }
        return sum;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n