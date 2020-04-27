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
 * -@TestCaseID: ThreadLocalEHwithInitial
 *- @TestCaseName: Thread_ThreadLocalEHwithInitial.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Check if ThreadLocal.withInitial(Supplier<? extends S> supplier) throws NullPointerException.
 *- @Brief: see below
 * -#step1: 创建参数supplier为null。
 * -#step2：调用withInitial(Supplier<? extends S> supplier)。
 * -#step3：确认会抛出NullPointerException异常。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadLocalEHwithInitial.java
 *- @ExecuteClass: ThreadLocalEHwithInitial
 *- @ExecuteArgs:
 */

public class ThreadLocalEHwithInitial {
    public static void main(String[] args) {
        try {
            ThreadLocal<Integer> threadLocal = ThreadLocal.withInitial(null);
        } catch (NullPointerException e) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n