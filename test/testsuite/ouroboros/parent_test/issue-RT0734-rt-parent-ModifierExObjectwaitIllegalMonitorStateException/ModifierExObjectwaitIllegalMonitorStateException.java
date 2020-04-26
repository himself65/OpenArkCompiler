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
 * -@TestCaseID: ModifierExObjectwaitIllegalMonitorStateException.java
 * -@TestCaseName: Exception in reflect Modifier: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create three methods, which call wait (), wait (millis), wait (millis, nanos)
 * -#step2: Call the run of the use case, and execute these three methods respectively
 * -#step3: Confirm that all methods throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ModifierExObjectwaitIllegalMonitorStateException.java
 * -@ExecuteClass: ModifierExObjectwaitIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.reflect.Modifier;

public class ModifierExObjectwaitIllegalMonitorStateException {
    static int res = 99;
    private static Modifier mf2 = new Modifier();

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
            result = modifierExObjectwaitIllegalMonitorStateException1();
        } catch (Exception e) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis)
        try {
            result = modifierExObjectwaitIllegalMonitorStateException2();
        } catch (Exception e) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis, int nanos)
        try {
            result = modifierExObjectwaitIllegalMonitorStateException3();
        } catch (Exception e) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 20;
        }

        if (result == 4 && ModifierExObjectwaitIllegalMonitorStateException.res == 96) {
            result = 0;
        }

        return result;
    }

    private static int modifierExObjectwaitIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        //
        // final void wait()
        try {
            mf2.wait();
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int modifierExObjectwaitIllegalMonitorStateException2() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis)
        long millis = 123;
        try {
            mf2.wait(millis);
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int modifierExObjectwaitIllegalMonitorStateException3() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis, int nanos)
        long millis = 123;
        int nanos = 10;
        try {
            mf2.wait(millis, nanos);
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ModifierExObjectwaitIllegalMonitorStateException.res = ModifierExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n