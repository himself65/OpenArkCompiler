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
 * -@TestCaseID: maple/runtime/rc/function/WeakAndCleanerRef.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Brief:functionTest
 * -#step1: 创建一个ThreadCleaner类的实例对象threadCleaner，创建一个StringBuffer类的变量stringBuffer并赋值为test；
 * -#step2: 以step1中的threadCleaner和stringBuffer为参数，调用Cleaner类的create()方法创建一个实例对象cleaner；
 * -#step3: 以ReferenceQueue类的实例对象rq和stringBuffer为参数，创建一个WeakReference类的对象并赋值给rp；
 * -#step4: 令stringBuffer等于null, 释放强引用；
 * -#step5: 调用cleaner的clean()方法，进行对象的清除和资源的释放；
 * -#step6: 让当前线程休眠2000ms；
 * -#step7: 重复执行step3一次；
 *- @Expect: ExpectResult\n
 *- @Priority: High
 *- @Source: WeakAndCleanerRef.java
 *- @ExecuteClass: WeakAndCleanerRef
 *- @ExecuteArgs:
 */

import sun.misc.Cleaner;

import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;
import java.lang.ref.WeakReference;

public class WeakAndCleanerRef {
    static Reference rp;
    static ReferenceQueue rq = new ReferenceQueue();
    static StringBuffer stringBuffer = new StringBuffer("test");

    static void addWeakAndCleanerRef() {
        rp = new WeakReference(stringBuffer, rq);
    }

    public static void main(String[] args) throws InterruptedException {
        ThreadCleaner threadCleaner = new ThreadCleaner();
        Cleaner cleaner = Cleaner.create(stringBuffer, threadCleaner);
        addWeakAndCleanerRef();
        stringBuffer = null;
        cleaner.clean();
        Thread.sleep(2000);
        addWeakAndCleanerRef();
        if (rp.get() == null) {
            System.out.println("ExpectResult");
        }
    }
}

class ThreadCleaner implements Runnable {
    @Override
    public void run() {
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n
