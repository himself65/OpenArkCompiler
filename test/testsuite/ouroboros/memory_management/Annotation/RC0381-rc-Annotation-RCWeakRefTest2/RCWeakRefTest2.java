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
 * -@TestCaseID:rc/unownedRef/RCWeakRefTest2.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Test the basic function of the @Weak
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: RCWeakRefTest2.java
 *- @ExecuteClass: RCWeakRefTest2
 *- @ExecuteArgs:
 */

import java.lang.ref.WeakReference;
import java.lang.reflect.Field;

import com.huawei.ark.annotation.Weak;

class Test_B {
    public void run() {
        System.out.println("ExpectResult");
    }
}

class Test_A {
    @Weak
    Test_B bb;
    @Weak
    volatile Test_B bv;

    public void test() {
        foo();
        new Thread(new TriggerRef()).start();

        try {
            Thread.sleep(5000);
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            bb.run();
        } catch (NullPointerException e) {
            System.out.println("ExpectResult");  //expect to be null
        }
        try {
            bv.run();
        } catch (NullPointerException e) {
            System.out.println("ExpectResult");  //expect to be null
        }
    }

    private void foo() {
        //bb = new Test_B();
        try {
            Field m = Test_A.class.getDeclaredField("bb");
            Test_B b1 = new Test_B();
            m.set(this, b1);
            Field m1 = Test_A.class.getDeclaredField("bv");
            m1.set(this, b1);

            Test_B temp1 = (Test_B) m.get(this);
            if (temp1 != b1) {
                System.out.println("error 1");
            }
            Test_B temp2 = (Test_B) m1.get(this);
            if (temp2 != b1) {
                System.out.println("error 1");
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    class TriggerRef implements Runnable {
        public void run() {
            for (int i = 0; i < 100; i++) {
                WeakReference ref = new WeakReference(new Object());
                try {
                    Thread.sleep(50);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
}

public class RCWeakRefTest2 {
    public static void main(String[] args) {
        new Test_A().test();
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n