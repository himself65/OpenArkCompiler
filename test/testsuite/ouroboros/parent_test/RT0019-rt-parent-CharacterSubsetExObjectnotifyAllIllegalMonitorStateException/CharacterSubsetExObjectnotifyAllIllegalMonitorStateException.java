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
 * -@TestCaseID: CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.java
 * -@TestCaseName: Exception in Character/Subset: final void notifyAll()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a private class to implement Runnable, call notifyAll () inside run
 * -#step2: Call the run of the use case, execute method 1, and call notifyAll () in the method
 * -#step3: Execute the start () method of the private class and wait 100 milliseconds
 * -#step4: Confirm method 1 throws an exception IllegalMonitorStateException, the class does not throw an exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.java
 * -@ExecuteClass: CharacterSubsetExObjectnotifyAllIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class CharacterSubsetExObjectnotifyAllIllegalMonitorStateException {
    static int res = 99;
    MySubset1 mf2 = new MySubset1("some subset");

    public static void main(String argv[]) {
        System.out.println(new CharacterSubsetExObjectnotifyAllIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = characterSubsetExObjectnotifyAllIllegalMonitorStateException1();
        } catch (Exception e) {
            CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res = CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res - 20;
        }
        Thread t1 = new Thread(new CharacterSubsetExObjectnotifyAllIllegalMonitorStateException11());
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
        if (result == 4 && CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res == 58) {
            result = 0;
        }
        return result;
    }

    private int characterSubsetExObjectnotifyAllIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        //  final void notifyAll()
        try {
            mf2.notifyAll();
            CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res = CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res = CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class CharacterSubsetExObjectnotifyAllIllegalMonitorStateException11 implements Runnable {
        //  final void notifyAll()

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (mf2) {
                try {
                    mf2.notifyAll();
                    CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res = CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res = CharacterSubsetExObjectnotifyAllIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}

class MySubset1 extends Character.Subset {
    MySubset1(String name) {
        super(name);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n