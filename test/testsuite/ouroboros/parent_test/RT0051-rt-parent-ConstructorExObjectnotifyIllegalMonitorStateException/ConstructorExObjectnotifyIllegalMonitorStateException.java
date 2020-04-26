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
 * -@TestCaseID: ConstructorExObjectnotifyIllegalMonitorStateException.java
 * -@TestCaseName: Exception in reflect /Constructor: final void notify()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a private class to implement Runnable, call notify() inside run
 * -#step2: Call the run of the use case, execute method 1, and call notify() in the method
 * -#step3: Execute the start () method of the private class and wait 100 milliseconds
 * -#step4: Confirm method 1 throws an exception IllegalMonitorStateException, the class does not throw an exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ConstructorExObjectnotifyIllegalMonitorStateException.java
 * -@ExecuteClass: ConstructorExObjectnotifyIllegalMonitorStateException
 * -@ExecuteArgs:
 */


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.Constructor;

public class ConstructorExObjectnotifyIllegalMonitorStateException {
    static int res = 99;
    private static Constructor<?> con = null;

    public static void main(String argv[]) throws NoSuchMethodException, SecurityException {
        con = SampleClass15.class.getConstructor(String.class);
        System.out.println(new ConstructorExObjectnotifyIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2;
        try {
            result = constructorExObjectnotifyIllegalMonitorStateExceptionTest1();
        } catch (Exception e) {
            ConstructorExObjectnotifyIllegalMonitorStateException.res = ConstructorExObjectnotifyIllegalMonitorStateException.res - 20;
        }
        Thread t1 = new Thread(new ConstructorExObjectnotifyIllegalMonitorStateException11(1));
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
        if (result == 4 && ConstructorExObjectnotifyIllegalMonitorStateException.res == 57) {
            result = 0;
        }
        return result;
    }

    private int constructorExObjectnotifyIllegalMonitorStateExceptionTest1() {
        int result1 = 4;
        try {
            con.notify();
            ConstructorExObjectnotifyIllegalMonitorStateException.res = ConstructorExObjectnotifyIllegalMonitorStateException.res - 50;

        } catch (IllegalMonitorStateException e2) {
            ConstructorExObjectnotifyIllegalMonitorStateException.res = ConstructorExObjectnotifyIllegalMonitorStateException.res - 2;
        }
        return result1;
    }

    private class ConstructorExObjectnotifyIllegalMonitorStateException11 implements Runnable {
        private int remainder;

        // final void notify()
        private ConstructorExObjectnotifyIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

    /**
     * Thread run fun
     */
        public void run() {
            synchronized (con) {
                try {
                    con.notify();
                    ConstructorExObjectnotifyIllegalMonitorStateException.res = ConstructorExObjectnotifyIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ConstructorExObjectnotifyIllegalMonitorStateException.res = ConstructorExObjectnotifyIllegalMonitorStateException.res - 30;
                }
            }
        }
    }

}

@CustomAnnotations15(name = "SampleClass", value = "Sample Class Annotation")
class SampleClass15 {
    private String sampleField;

    @CustomAnnotations15(name = "sampleConstructor", value = "Sample Constructor Annotation")
    public SampleClass15(String s) {

    }

    public String getSampleField() {
        return sampleField;
    }

    public void setSampleField(String sampleField) {
        this.sampleField = sampleField;
    }
}

@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.TYPE, ElementType.METHOD, ElementType.CONSTRUCTOR})
@interface CustomAnnotations15 {
    String name();

    String value();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n