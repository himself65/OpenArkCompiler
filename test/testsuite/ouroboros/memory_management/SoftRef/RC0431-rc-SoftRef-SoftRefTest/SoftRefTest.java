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
 * -@TestCaseID:SoftRefTest.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: 测试SoftRefernece的基本内存管理方式：在正常情况下，SoftReference对象不会被回收掉
 *- @Brief:functionTest
 * -#step1：创建一个SoftReference对象，并将其放到ReferneceQueue中，该对象不应该被释放；
 * -#step2：封装一个函数生成一下局部的SoftReference对象
 * -#step3：再次确认该对象不应该被释放，且它所在的ReferenceQueue中没有即将回收的Reference对象
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: SoftRefTest.java
 *- @ExecuteClass: SoftRefTest
 *- @ExecuteArgs:
 *
 */

import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;
import java.lang.ref.SoftReference;

public class SoftRefTest {
    static Reference rp;
    static ReferenceQueue rq = new ReferenceQueue();
    static int a = 100;

    static void setSoftReference() {
        rp = new SoftReference<Object>(new Object(), rq);
        if (rp.get() == null) {
            System.out.println("Error Result when first check ");
            a++;
        }
    }

    public static void main(String[] args) throws Exception {
        setSoftReference();
        new Thread(new TriggerRP()).start();

        if (rp.get() == null) {
            System.out.println("Error Result when second check ");
            a++;
        }
        Reference r = rq.poll();
        if (r != null) {
            System.out.println("Error Result when checking ReferenceQueue");
            a++;
        }
        if (a == 100) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("ErrorResult finally");
        }

    }

    static class TriggerRP implements Runnable {
        public void run() {
            for (int i = 0; i < 60; i++) {
                SoftReference sr = new SoftReference(new Object());
                try {
                    Thread.sleep(50);
                } catch (Exception e) {
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n