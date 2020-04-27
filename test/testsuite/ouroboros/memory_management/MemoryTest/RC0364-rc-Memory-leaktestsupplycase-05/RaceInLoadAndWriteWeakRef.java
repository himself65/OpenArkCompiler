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
 * -@TestCaseID: maple/runtime/rc/function/RaceInLoadAndWriteWeakRef.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Brief:functionTest
 * -#step1: 创建一个WeakReference类的对象并赋值给rp，经判断rp.get()不为null，最后令stringBuffer等于null；
 * -#step2: 以setWeakRef为参数，分别创建ThreadLoadWeakRef、ThreadWriteWeakRef类的实例对象threadLoadWeakRef和
 *          threadWriteWeakRef；
 * -#step3: 调用threadLoadWeakRef的run()方法，即将setWeakRef.rp的值赋值给rp_load；
 * -#step4: 调用Runtime.getRuntime().gc()进行垃圾回收；
 * -#step5: 重复执行step3~step4一次；
 * -#step6: 再次执行step3一次；
 * -#step7: 调用threadWriteWeakRef的run()方法，即将rp_write的值赋值给setWeakRef.rp；
 * -#step8: 调用Runtime.getRuntime().gc()进行垃圾回收；
 * -#step9: 重复执行step7一次；
 * -#step10: 调用Runtime.getRuntime().gc()进行垃圾回收；
 * -#step11: 重复执行步骤1~9一次；
 *- @Expect:
 *- @Priority: High
 *- @Source: RaceInLoadAndWriteWeakRef.java
 *- @ExecuteClass: RaceInLoadAndWriteWeakRef
 *- @ExecuteArgs:
 */

import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;
import java.lang.ref.WeakReference;

public class RaceInLoadAndWriteWeakRef {
    static int NUM = 100;

    public static void main(String[] args) {
        test();
        Runtime.getRuntime().gc();
        test();
        System.out.println("ExpectResult");
    }

    static void test() {
        SetWeakRef setWeakRef = new SetWeakRef();
        setWeakRef.setWeakRef();
        ThreadLoadWeakRef threadLoadWeakRef = new ThreadLoadWeakRef(setWeakRef);
        ThreadWriteWeakRef threadWriteWeakRef = new ThreadWriteWeakRef(setWeakRef);
        for (int i = 0; i < NUM; i++) {
            threadLoadWeakRef.run();
            Runtime.getRuntime().gc();
            threadLoadWeakRef.run();
            Runtime.getRuntime().gc();
            threadLoadWeakRef.run();

            threadWriteWeakRef.run();
            Runtime.getRuntime().gc();
            threadWriteWeakRef.run();
        }
    }
}

class SetWeakRef {
    static Reference rp;
    static ReferenceQueue rq = new ReferenceQueue();
    static StringBuffer stringBuffer = new StringBuffer("Weak");

    static void setWeakRef() {
        stringBuffer = new StringBuffer("Weak");
        rp = new WeakReference(stringBuffer, rq);
        if (rp.get() == null) {
            System.out.println("error");
        }
        stringBuffer = null;
    }
}

class ThreadLoadWeakRef implements Runnable {
    SetWeakRef setWeakRef;

    public ThreadLoadWeakRef(SetWeakRef setWeakRef) {
        this.setWeakRef = setWeakRef;
    }

    static Reference rp_load;

    @Override
    public void run() {
        rp_load = setWeakRef.rp;
    }
}

class ThreadWriteWeakRef implements Runnable {
    SetWeakRef setWeakRef;

    public ThreadWriteWeakRef(SetWeakRef setWeakRef) {
        this.setWeakRef = setWeakRef;
    }

    static WeakReference rp_write;

    @Override
    public void run() {
        setWeakRef.rp = rp_write;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult
