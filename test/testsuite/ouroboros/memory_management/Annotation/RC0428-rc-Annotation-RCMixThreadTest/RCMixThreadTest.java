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
 * -@TestCaseID:maple/runtime/rc/annotation/Permanent/RCMixThreadTest
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:多线程下调用Weak 或Unowned和Permanent一起用在一个对象的case，验证是否能够正常被释放
 *- @Brief:functionTest
 * 多线程下调用Weak 或Unowned和Permanent一起用在一个对象的case，验证是否能够正常被释放
 *- @Expect:ExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: RCMixThreadTest.java
 *- @ExecuteClass: RCMixThreadTest
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;
import java.util.ArrayList;

import com.huawei.ark.annotation.*;


public class RCMixThreadTest {
    public static void main(String[] args) throws InterruptedException {
        rc_testcase_main_wrapper();
    }

    public static void rc_testcase_main_wrapper() throws InterruptedException {
        RCMixTest_Weak rcMixTest_weak = new RCMixTest_Weak();
        RCMixTest_Weak rcMixTest_weak2 = new RCMixTest_Weak();
        RCMixTest_Weak rcMixTest_weak3 = new RCMixTest_Weak();
        RCMixTest_Weak rcMixTest_weak4 = new RCMixTest_Weak();
        RCMixTest_Weak rcMixTest_weak5 = new RCMixTest_Weak();

        rcMixTest_weak.start();
        rcMixTest_weak2.start();
        rcMixTest_weak3.start();
        rcMixTest_weak4.start();
        rcMixTest_weak5.start();

        rcMixTest_weak.join();
        rcMixTest_weak2.join();
        rcMixTest_weak3.join();
        rcMixTest_weak4.join();
        rcMixTest_weak5.join();
    }
}


class RCMixTest_Weak extends Thread {

    public void run() {
        new Test_A_Weak().test();
    }
}


class Test_A_Weak {
    @Weak
    Test_B_Weak bb;
    Test_B_Weak bb2;

    public void test() {
        foo();
        try {
            Thread.sleep(5000);
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            bb.run();
        } catch (NullPointerException e) {
            System.out.println("NullPointerException");
        }
        bb2.run();
    }

    private void foo() {
        bb = new @Permanent Test_B_Weak();
        bb2 = new Test_B_Weak();
    }
}

class Test_B_Weak {
    public void run() {
        System.out.println("ExpectResult");
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\nExpectResult\n