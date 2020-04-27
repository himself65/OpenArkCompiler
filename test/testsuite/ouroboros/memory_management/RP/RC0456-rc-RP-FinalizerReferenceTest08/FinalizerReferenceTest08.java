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
 * -@TestCaseID: Maple_MemoryManagement2.0_FinalizerReferenceTest08
 *- @TestCaseName: FinalizerReferenceTest08
 *- @TestCaseType: Function Testing for FinalizerReference Test
 *- @RequirementName: 运行时支持GCOnly
 *- @Brief:测试正常情况下，Cleaner+FinalizerReference指向的对象内存回收正确。
 *  -#step1: 创建带有重构finalize方法的类ReferenceTest08,
 *  -#step2: 该实例的对象gc回收时，finalize方法会将其关联到一个虚引用上，判断无问题。
 *  -#step3: 触发用户GC，虚引用无法通过get获得finalize方法不再被调用。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: FinalizerReferenceTest08.java
 *- @ExecuteClass: FinalizerReferenceTest08
 *- @ExecuteArgs:
 */

import sun.misc.Cleaner;

public class FinalizerReferenceTest08 {
    static Cleaner cleaner;
    static int check = 2;

    public static void main(String[] args) {
        new ReferenceTest08(10);
        Runtime.getRuntime().gc();
        Runtime.getRuntime().runFinalization();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().runFinalization();
    }
}

class ReferenceTest08 {
    String[] stringArray;

    public ReferenceTest08(int length) {
        stringArray = new String[length];
        for (int i = 0; i < length; i++) {
            stringArray[i] = "test" + i;
        }
    }

    @Override
    public void finalize() {
        System.out.println("ExpectResult");
        Cleaner.create(this, null);
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n