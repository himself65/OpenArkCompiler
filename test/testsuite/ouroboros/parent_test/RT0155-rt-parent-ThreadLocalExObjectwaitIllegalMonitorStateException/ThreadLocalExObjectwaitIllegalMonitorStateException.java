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
 * -@TestCaseID: ThreadLocalExObjectwaitIllegalMonitorStateException.java
 * -@TestCaseName: Exception in ThreadLocal: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare current thread is not the owner of the object's monitor
 * -#step2:Test ThreadLocal api wait extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadLocalExObjectwaitIllegalMonitorStateException.java
 * -@ExecuteClass: ThreadLocalExObjectwaitIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadLocalExObjectwaitIllegalMonitorStateException {
    static int res = 99;
    private static ThreadLocal<Object> tl1 = new ThreadLocal<>();

    public static void main(String argv[]) {
        System.out.println(run());
    }

    /**
     * main test fun
     * @return status code
     */
    public static int run() {
        int result = 2; /*STATUS_FAILED*/
        // final void wait()
        try {
            result = threadLocalExObjectwaitIllegalMonitorStateException1();
        } catch (Exception e) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis)
        try {
            result = threadLocalExObjectwaitIllegalMonitorStateException2();
        } catch (Exception e) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis, int nanos)
        try {
            result = threadLocalExObjectwaitIllegalMonitorStateException3();
        } catch (Exception e) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 20;
        }

        if (result == 4 && ThreadLocalExObjectwaitIllegalMonitorStateException.res == 96) {
            result = 0;
        }
        return result;
    }

    private static int threadLocalExObjectwaitIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait()
        try {
            tl1.wait();
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int threadLocalExObjectwaitIllegalMonitorStateException2() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis)
        long millis = 123;
        try {
            tl1.wait(millis);
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int threadLocalExObjectwaitIllegalMonitorStateException3() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis, int nanos)
        long millis = 123;
        int nanos = 10;
        try {
            tl1.wait(millis, nanos);
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThreadLocalExObjectwaitIllegalMonitorStateException.res = ThreadLocalExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n