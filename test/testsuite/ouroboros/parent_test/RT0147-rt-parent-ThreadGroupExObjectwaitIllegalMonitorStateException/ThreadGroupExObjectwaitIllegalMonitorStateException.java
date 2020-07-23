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
 * -@TestCaseID: ThreadGroupExObjectwaitIllegalMonitorStateException.java
 * -@TestCaseName: Exception in ThreadGroup: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare the current thread is not the owner of the object's monitor.
 * -#step2:Test api wait extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadGroupExObjectwaitIllegalMonitorStateException.java
 * -@ExecuteClass: ThreadGroupExObjectwaitIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.ThreadGroup;

public class ThreadGroupExObjectwaitIllegalMonitorStateException {
    static int res = 99;
    private static ThreadGroup gr1 = new ThreadGroup("Thread8023");

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
            result = threadGroupExObjectwaitIllegalMonitorStateException1();
        } catch (Exception e) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis)
        try {
            result = threadGroupExObjectwaitIllegalMonitorStateException2();
        } catch (Exception e) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis, int nanos)
        try {
            result = threadGroupExObjectwaitIllegalMonitorStateException3();
        } catch (Exception e) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 20;
        }

        if (result == 4 && ThreadGroupExObjectwaitIllegalMonitorStateException.res == 96) {
            result = 0;
        }

        return result;
    }

    private static int threadGroupExObjectwaitIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait()
        try {
            gr1.wait();
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int threadGroupExObjectwaitIllegalMonitorStateException2() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis)
        long millis = 123;
        try {
            gr1.wait(millis);
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int threadGroupExObjectwaitIllegalMonitorStateException3() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis, int nanos)
        long millis = 123;
        int nanos = 10;
        try {
            gr1.wait(millis, nanos);
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ThreadGroupExObjectwaitIllegalMonitorStateException.res = ThreadGroupExObjectwaitIllegalMonitorStateException.res - 1;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n