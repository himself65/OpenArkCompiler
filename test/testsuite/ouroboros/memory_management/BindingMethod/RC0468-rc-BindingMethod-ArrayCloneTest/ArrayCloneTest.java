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
 * -@TestCaseID: Maple_MemoryManagement2.0_ArrayCloneTest
 *- @TestCaseName: ArrayCloneTest
 *- @TestCaseType: Function Testing for MemoryBindingMethod Test
 *- @RequirementName: 运行时支持GCOnly
 *- @Brief:clone()函数专项测试：验证数组的clone()方法的基本功能正常。
 *  -#step1:测试正常数组拷贝
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: ArrayCloneTest.java
 *- @ExecuteClass: ArrayCloneTest
 *- @ExecuteArgs:
 */

public class ArrayCloneTest {
    public static void main(String[] args) {
        if (functionTest01()) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("ErrorResult");
        }
    }

    private static boolean functionTest01() {
        String[] test1 = {"test1", "test2", "test3", "test4", "test5", "test6", "test7", "test8", "test9", "test10"};
        String[] test2 = test1.clone();
        if (test2 != test1 && test2.length == 10 && test2[0].equals("test1") && test2[9].equals("test10")) {
            return true;
        }
        return false;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n