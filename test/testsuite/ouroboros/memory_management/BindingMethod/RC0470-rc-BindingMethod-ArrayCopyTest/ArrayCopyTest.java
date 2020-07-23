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
 * -@TestCaseID: Maple_MemoryManagement2.0_ArrayCopyTest
 *- @TestCaseName: ArrayCopyTest
 *- @TestCaseType: Function Testing for MemoryBindingMethod Test
 *- @RequirementName: 运行时支持GCOnly
 *- @Brief:ArrayCopy函数专项测试：验证ArrayCopy方法的基本功能正常。
 *  -#step1:测试正常数组拷贝
 *  -#step2:测试拷贝过程中出现ArrayStoreException异常
 *  -#step3:测试拷贝过程中出现ArrayIndexOutOfBoundsException异常
 *  -#step4:测试拷贝过程中出现NullPointerException异常
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: ArrayCopyTest.java
 *- @ExecuteClass: ArrayCopyTest
 *- @ExecuteArgs:
 */

public class ArrayCopyTest {
    public static void main(String[] args) {
        if (functionTest01() && functionTest02() && functionTest03() && functionTest04()) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("ErrorResult");
        }
    }

    private static boolean functionTest01() {
        String[] test1 = {"test1", "test2", "test3", "test4", "test5", "test6", "test7", "test8", "test9", "test10"};
        String[] test2 = new String[10];
        System.arraycopy(test1, 0, test2, 0, 10);
        if (test2[0].equals("test1") && test2[9].equals("test10")) {
            return true;
        }
        return false;
    }

    private static boolean functionTest02() {
        try {
            String[] test1 = {"test1", "test2", "test3", "test4", "test5", "test6", "test7", "test8", "test9", "test10"};
            int[] test2 = new int[10];
            System.arraycopy(test1, 0, test2, 0, 10);

        } catch (ArrayStoreException e) {
            return true;
        }
        return false;
    }

    private static boolean functionTest03() {
        try {
            String[] test1 = {"test1", "test2", "test3", "test4", "test5", "test6", "test7", "test8", "test9", "test10"};
            String[] test2 = new String[9];
            System.arraycopy(test1, 0, test2, 0, 10);

        } catch (ArrayIndexOutOfBoundsException e) {
            return true;
        }
        return false;
    }

    private static boolean functionTest04() {
        try {
            String[] test1 = {"test1", "test2", "test3", "test4", "test5", "test6", "test7", "test8", "test9", "test10"};
            String[] test2 = null;
            System.arraycopy(test1, 0, test2, 0, 10);

        } catch (NullPointerException e) {
            return true;
        }
        return false;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n