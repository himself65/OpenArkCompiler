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
 * -@TestCaseID: ConstructorExObjectwaitIllegalArgumentException.java
 * -@TestCaseName:  final void wait(long millis, int nanos)/ final void wait(long millis)/ final void wait()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create 2 private classes to implement Runnable, and call wait (millis), wait (millis, nanos) respectively in run
 * -#step2: Call the run of the use case and execute the start () method of the private class 1.
 * -#step3: Execute the start () method of private class 2
 * -#step4: Wait 1000 ms
 * -#step5: Confirm that all classes throw IllegalArgumentException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ConstructorExObjectwaitIllegalArgumentException.java
 * -@ExecuteClass: ConstructorExObjectwaitIllegalArgumentException
 * -@ExecuteArgs:
 */


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.Constructor;

public class ConstructorExObjectwaitIllegalArgumentException {
    static int res = 99;
    private static Constructor<?> con = null;

    public static void main(String argv[]) throws SecurityException, NoSuchMethodException {
        con = SampleClass16.class.getConstructor(String.class);
        System.out.println(new ConstructorExObjectwaitIllegalArgumentException().run());
    }

    private class ConstructorExObjectwaitIllegalArgumentException11 implements Runnable {
        /**
         * Thread run fun
         */
        public void run() {
            synchronized (con) {
                con.notifyAll();
                try {
                    con.wait();
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 15;
                } catch (InterruptedException e1) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 10;
                }
            }
        }
    }

    private class ConstructorExObjectwaitIllegalArgumentException21 implements Runnable {
        // final void wait(long millis)
        long millis = 10000;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (con) {
                con.notifyAll();
                try {
                    con.wait(millis);
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 15;
                } catch (InterruptedException e1) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 5;
                }
            }
        }
    }

    private class ConstructorExObjectwaitIllegalArgumentException31 implements Runnable {
        // final void wait(long millis, int nanos)
        long millis = 10000;
        int nanos = 100;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (con) {
                con.notifyAll();
                try {
                    con.wait(millis, nanos);
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 15;
                } catch (InterruptedException e1) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ConstructorExObjectwaitIllegalArgumentException.res = ConstructorExObjectwaitIllegalArgumentException.res - 5;
                }
            }
        }
    }

    /**
     * sleep fun
     *
     * @param slpnum wait time
     */
    public void sleep(int slpnum) {
        try {
            Thread.sleep(slpnum);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        // check api normal
        // final void wait()
        Thread t1 = new Thread(new ConstructorExObjectwaitIllegalArgumentException11());
        // final void wait(long millis)
        Thread t3 = new Thread(new ConstructorExObjectwaitIllegalArgumentException21());
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ConstructorExObjectwaitIllegalArgumentException31());
        t1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t3.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t5.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t1.start();
        sleep(1000);
        t1.interrupt();
        sleep(1000);
        t3.start();
        sleep(1000);
        t3.interrupt();
        sleep(1000);
        t5.start();
        sleep(1000);
        t5.interrupt();
        sleep(1000);

        if (result == 2 && ConstructorExObjectwaitIllegalArgumentException.res == 96) {
            result = 0;
        }
        return result;
    }
}

@CustomAnnotations16(name = "SampleClass", value = "Sample Class Annotation")
class SampleClass16 {
    private String sampleField;

    @CustomAnnotations16(name = "sampleConstructor", value = "Sample Constructor Annotation")
    public SampleClass16(String str) {

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
@interface CustomAnnotations16 {
    String name();

    String value();
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n