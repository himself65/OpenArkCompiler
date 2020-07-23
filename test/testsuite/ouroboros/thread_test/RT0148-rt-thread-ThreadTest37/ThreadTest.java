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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test for void sleep(long millis) and sleep(long millis, int nanos) with both valid and
 *                      invalid value
 *- @Brief: see below
 * case1:
 * -#step1: Test void sleep(long millis) with legal or illegal param.
 * -#step2: Check that IllegalArgumentException was threw when param is illegal.
 * case2:
 * -#step1: Test sleep(long millis, int nanos) with legal or illegal param.
 * -#step2: Check that IllegalArgumentException was threw when param is illegal.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        long millis = 2000;

        try {
            Thread.sleep(millis);
            System.out.println("sleep(long millis) OK");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException for sleep(long millis) as: millis < 0");
        }

        try {
            Thread.sleep(-2);
            System.out.println("sleep(long millis) OK");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException for sleep(long millis) as: millis < 0");
        }

        int nanos = 123456;

        try {
            Thread.sleep(millis, nanos);
            System.out.println("sleep(long millis, int nanos) OK");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException for sleep(long millis, int nanos) as: millis < 0 or nanos < 0 or nanos > 999999");
        }

        try {
            Thread.sleep(millis, -1);
            System.out.println("sleep(long millis, int nanos) OK");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException for sleep(long millis, int nanos) as: nanos < 0");
        }

        try {
            Thread.sleep(millis, 1123456);
            System.out.println("sleep(long millis, int nanos) OK");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException for sleep(long millis, int nanos) as: nanos > 999999");
        }

        try {
            Thread.sleep(-3, nanos);
            System.out.println("sleep(long millis, int nanos) OK");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException for sleep(long millis, int nanos) as: millis < 0");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full sleep(long millis) OK\nIllegalArgumentException for sleep(long millis) as: millis < 0\nsleep(long millis, int nanos) OK\nIllegalArgumentException for sleep(long millis, int nanos) as: nanos < 0\nIllegalArgumentException for sleep(long millis, int nanos) as: nanos > 999999\nIllegalArgumentException for sleep(long millis, int nanos) as: millis < 0\n