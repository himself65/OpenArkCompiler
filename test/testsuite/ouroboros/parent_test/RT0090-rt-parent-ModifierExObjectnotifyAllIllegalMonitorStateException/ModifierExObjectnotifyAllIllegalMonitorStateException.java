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
 * -@TestCaseID: ModifierExObjectnotifyAllIllegalMonitorStateException.java
 * -@TestCaseName: Exception in reflect Modifier:  final void notifyAll()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a private class to implement Runnable, call notifyAll() inside run
 * -#step2: Call the run of the use case, execute method 1, and call notifyAll () in the method
 * -#step3: Execute the start () method of the private class and wait 100 milliseconds
 * -#step4: Confirm method 1 throws an exception IllegalMonitorStateException, the class does not throw an exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ModifierExObjectnotifyAllIllegalMonitorStateException.java
 * -@ExecuteClass: ModifierExObjectnotifyAllIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.reflect.Modifier;

public class ModifierExObjectnotifyAllIllegalMonitorStateException {
    static int res = 99;
    Modifier mf2 = new Modifier();

    public static void main(String argv[]) {
        System.out.println(new ModifierExObjectnotifyAllIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = modifierExObjectnotifyAllIllegalMonitorStateException1();
        } catch (Exception e) {
            ModifierExObjectnotifyAllIllegalMonitorStateException.res = ModifierExObjectnotifyAllIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new ModifierExObjectnotifyAllIllegalMonitorStateException11(1));
        t1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t1.start();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (result == 4 && ModifierExObjectnotifyAllIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int modifierExObjectnotifyAllIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        //  final void notifyAll()
        try {
            mf2.notifyAll();
            ModifierExObjectnotifyAllIllegalMonitorStateException.res = ModifierExObjectnotifyAllIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            ModifierExObjectnotifyAllIllegalMonitorStateException.res = ModifierExObjectnotifyAllIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class ModifierExObjectnotifyAllIllegalMonitorStateException11 implements Runnable {
        //  final void notifyAll()
        private int remainder;

        private ModifierExObjectnotifyAllIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (mf2) {
                try {
                    mf2.notifyAll();
                    ModifierExObjectnotifyAllIllegalMonitorStateException.res = ModifierExObjectnotifyAllIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ModifierExObjectnotifyAllIllegalMonitorStateException.res = ModifierExObjectnotifyAllIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n