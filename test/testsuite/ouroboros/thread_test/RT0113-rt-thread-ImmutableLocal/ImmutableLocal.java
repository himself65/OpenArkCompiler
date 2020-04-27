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
 * -@TestCaseID: ImmutableLocal
 *- @TestCaseName: Thread_ImmutableLocal.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Confirm ThreadLocal.set() usage is not a side effect of get().
 *- @Brief: see below
 * -#step1：定义继承ThreadLocal类的抽象类ImmutableThreadLocal，定义抛出new RuntimeException的set()和私有的受保护
 *          initialValue()。
 * -#step2：通过new得到一个私有的不可变的线程对象cache，运行initialValue()，调用getName()获取当前执行线程对象的引用。
 * -#step3：在主函数中调用cache.get()获取对象引用，确认获取正确。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ImmutableLocal.java
 *- @ExecuteClass: ImmutableLocal
 *- @ExecuteArgs:
 */

public class ImmutableLocal {
    private static final ThreadLocal cache = new ImmutableThreadLocal() {
        public Object initialValue() {
            return Thread.currentThread().getName();
        }
    };

    public static void main(final String[] args) {
        if (cache.get().equals("main")) {
            System.out.println(0);
        }
    }

    /**
     * {@link ThreadLocal} guaranteed to always return the same reference.
     */
    abstract public static class ImmutableThreadLocal extends ThreadLocal {
        public void set(final Object value) {
            throw new RuntimeException("ImmutableThreadLocal set called");
        }

        // Force override
        abstract protected Object initialValue();
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n