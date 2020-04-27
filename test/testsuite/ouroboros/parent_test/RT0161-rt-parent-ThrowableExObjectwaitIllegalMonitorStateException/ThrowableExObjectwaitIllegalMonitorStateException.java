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
 * -@TestCaseID: ThrowableExObjectwaitIllegalMonitorStateException.java
 * -@TestCaseName: Exception in Throwable: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare current thread is not the owner of the object's monitor
 * -#step2:Test Throwable api notifyAll extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThrowableExObjectwaitIllegalMonitorStateException.java
 * -@ExecuteClass: ThrowableExObjectwaitIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Throwable;

public class ThrowableExObjectwaitIllegalMonitorStateException {
    static int res = 99;
    private static Throwable cause = new Throwable("detailed message of cause");

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
            result = throwableExObjectwaitIllegalMonitorStateException1();
        } catch (Exception e) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis)
        try {
            result = throwableExObjectwaitIllegalMonitorStateException2();
        } catch (Exception e) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis, int nanos)
        try {
            result = throwableExObjectwaitIllegalMonitorStateException3();
        } catch (Exception e) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 20;
        }

        if (result == 4 && ThrowableExObjectwaitIllegalMonitorStateException.res == 96) {
            result = 0;
        }

        return result;
    }

    private static int throwableExObjectwaitIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait()
        try {
            cause.wait();
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int throwableExObjectwaitIllegalMonitorStateException2() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis)
        long millis = 123;
        try {
            cause.wait(millis);
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int throwableExObjectwaitIllegalMonitorStateException3() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis, int nanos)
        long millis = 123;
        int nanos = 10;
        try {
            cause.wait(millis, nanos);
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThrowableExObjectwaitIllegalMonitorStateException.res = ThrowableExObjectwaitIllegalMonitorStateException.res - 1;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n