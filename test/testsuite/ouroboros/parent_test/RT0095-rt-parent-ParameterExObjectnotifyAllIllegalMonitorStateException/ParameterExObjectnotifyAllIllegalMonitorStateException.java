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
 * -@TestCaseID: ParameterExObjectnotifyAllIllegalMonitorStateException.java
 * -@TestCaseName: Exception in reflect Parameter:  final void notifyAll()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a private class to implement Runnable, call notifyAll () inside run
 * -#step2: Call the run of the use case, execute method 1, and call notifyAll () in the method
 * -#step3: Execute the start () method of the private class and wait 100 milliseconds
 * -#step4: Confirm method 1 throws an exception IllegalMonitorStateException, the class does not throw an exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ParameterExObjectnotifyAllIllegalMonitorStateException.java
 * -@ExecuteClass: ParameterExObjectnotifyAllIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.reflect.Parameter;
import java.lang.reflect.Method;

public class ParameterExObjectnotifyAllIllegalMonitorStateException {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ParameterExObjectnotifyAllIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = parameterExObjectnotifyAllIllegalMonitorStateException1();
        } catch (Exception e) {
            ParameterExObjectnotifyAllIllegalMonitorStateException.res = ParameterExObjectnotifyAllIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new ParameterExObjectnotifyAllIllegalMonitorStateException11(1));
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

        if (result == 4 && ParameterExObjectnotifyAllIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int parameterExObjectnotifyAllIllegalMonitorStateException1() throws NoSuchMethodException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void notifyAll()
        Method method = Parameter03a.class.getMethod("getName", new Class[]{String.class});
        Parameter[] parameters = method.getParameters();

        try {
            parameters[0].notifyAll();
            ParameterExObjectnotifyAllIllegalMonitorStateException.res = ParameterExObjectnotifyAllIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            ParameterExObjectnotifyAllIllegalMonitorStateException.res = ParameterExObjectnotifyAllIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class ParameterExObjectnotifyAllIllegalMonitorStateException11 implements Runnable {
        // final void notifyAll()
        private int remainder;

        private ParameterExObjectnotifyAllIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            Method me = null;
            try {
                me = Parameter03a.class.getMethod("getName", new Class[]{String.class});
            } catch (NoSuchMethodException | SecurityException e) {
                e.printStackTrace();
            }
            Parameter[] parameters = me.getParameters();
            synchronized (parameters[0]) {
                try {
                    parameters[0].notifyAll();
                    ParameterExObjectnotifyAllIllegalMonitorStateException.res = ParameterExObjectnotifyAllIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ParameterExObjectnotifyAllIllegalMonitorStateException.res = ParameterExObjectnotifyAllIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}

class Parameter03a {
    private String name = "default";

    public String getName(String str1) {
        return str1;
    }

}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n