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
 * -@TestCaseID:maple/runtime/rc/function/RCAnnotationTwoCycle.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Cycle_B_2_00080 for @Weak and @Unowned Test.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RCAnnotationTwoCycle.java
 *- @ExecuteClass: RCAnnotationTwoCycle
 *- @ExecuteArgs:
 */

import com.huawei.ark.annotation.Unowned;
import com.huawei.ark.annotation.Weak;

class Cycle_B_2_00080_A1 {
    Cycle_B_2_00080_A2 a2_0;
    Cycle_B_2_00080_A5 a5_0;
    int a;
    int sum;

    Cycle_B_2_00080_A1() {
        a2_0 = null;
        a5_0 = null;
        a = 1;
        sum = 0;
    }

    void add() {
        sum = a + a2_0.a;
    }
}


class Cycle_B_2_00080_A2 {
    Cycle_B_2_00080_A3 a3_0;
    Cycle_B_2_00080_A4 a4_0;
    int a;
    int sum;

    Cycle_B_2_00080_A2() {
        a3_0 = null;
        a4_0 = null;
        a = 2;
        sum = 0;
    }

    void add() {
        sum = a + a3_0.a;
    }
}


class Cycle_B_2_00080_A3 {
    @Weak
    Cycle_B_2_00080_A1 a1_0;
    @Unowned
    Cycle_B_2_00080_A5 a5_0;
    int a;
    int sum;

    Cycle_B_2_00080_A3() {
        a1_0 = null;
        a5_0 = null;
        a = 3;
        sum = 0;
    }

    void add() {
        sum = a + a1_0.a;
    }
}


class Cycle_B_2_00080_A4 {
    @Unowned
    Cycle_B_2_00080_A3 a3_0;
    @Unowned
    Cycle_B_2_00080_A2 a2_0;
    int a;
    int sum;

    Cycle_B_2_00080_A4() {
        a3_0 = null;
        a2_0 = null;
        a = 4;
        sum = 0;
    }

    void add() {
        sum = a + a3_0.a;
    }
}


class Cycle_B_2_00080_A5 {
    @Unowned
    Cycle_B_2_00080_A4 a4_0;
    int a;
    int sum;

    Cycle_B_2_00080_A5() {
        a4_0 = null;
        a = 5;
        sum = 0;
    }

    void add() {
        sum = a + a4_0.a;
    }
}

public class RCAnnotationTwoCycle {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
    }

    private static void rc_testcase_main_wrapper() {
        Cycle_B_2_00080_A1 a1_0 = new Cycle_B_2_00080_A1();
        a1_0.a2_0 = new Cycle_B_2_00080_A2();
        a1_0.a2_0.a3_0 = new Cycle_B_2_00080_A3();
        Cycle_B_2_00080_A4 a4_0 = new Cycle_B_2_00080_A4();
        a4_0.a2_0 = a1_0.a2_0;
        a1_0.a2_0.a3_0.a1_0 = a1_0;
        a4_0.a3_0 = a1_0.a2_0.a3_0;
        a1_0.a5_0 = new Cycle_B_2_00080_A5();
        a1_0.a2_0.a3_0.a5_0 = a1_0.a5_0;
        a4_0.a3_0.a5_0 = a1_0.a5_0;
        a4_0.a3_0.a5_0.a4_0 = a4_0;
        a1_0.add();
        a1_0.a2_0.add();
        a1_0.a2_0.a3_0.add();
        a4_0.add();
        a1_0.a5_0.add();
        int nsum = (a1_0.sum + a1_0.a2_0.sum + a1_0.a2_0.a3_0.sum + a4_0.sum + a1_0.a5_0.sum);
        //System.out.println(nsum);
        if (nsum == 28)
            System.out.println("ExpectResult");
    }

}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n