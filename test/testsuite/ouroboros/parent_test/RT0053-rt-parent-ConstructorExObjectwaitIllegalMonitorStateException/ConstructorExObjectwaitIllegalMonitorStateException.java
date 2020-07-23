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
 * -@TestCaseName: final void wait(long millis, int nanos)/ final void wait(long millis)/ final void wait()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create three methods, which call wait (), wait (millis), wait (millis, nanos)
 * -#step2: Call the run of the use case, and execute these three methods respectively
 * -#step3: Confirm that all methods throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ConstructorExObjectwaitIllegalMonitorStateException.java
 * -@ExecuteClass: ConstructorExObjectwaitIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.Constructor;

public class ConstructorExObjectwaitIllegalMonitorStateException {
    static int res = 99;
    private static Constructor<?> con = null;

    public static void main(String argv[]) throws SecurityException, NoSuchMethodException {
        con = SampleClass17.class.getConstructor(String.class);
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
            result = constructorExObjectwaitIllegalMonitorStateException1();
        } catch (Exception e) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis)
        try {
            result = constructorExObjectwaitIllegalMonitorStateException2();
        } catch (Exception e) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 20;
        }
        // final void wait(long millis, int nanos)
        try {
            result = constructorExObjectwaitIllegalMonitorStateException3();
        } catch (Exception e) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 20;
        }

        if (result == 4 && ConstructorExObjectwaitIllegalMonitorStateException.res == 96) {
            result = 0;
        }

        return result;
    }

    private static int constructorExObjectwaitIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait()
        try {
            con.wait();
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int constructorExObjectwaitIllegalMonitorStateException2() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis)
        long millis = 123;
        try {
            con.wait(millis);
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private static int constructorExObjectwaitIllegalMonitorStateException3() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void wait(long millis, int nanos)
        long millis = 123;
        int nanos = 10;
        try {
            con.wait(millis, nanos);
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 10;
        } catch (InterruptedException e1) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 30;
        } catch (IllegalMonitorStateException e2) {
            ConstructorExObjectwaitIllegalMonitorStateException.res = ConstructorExObjectwaitIllegalMonitorStateException.res - 1;
        }

        return result1;
    }
}

@CustomAnnotations17(name = "SampleClass", value = "Sample Class Annotation")
class SampleClass17 {
    private String sampleField;

    @CustomAnnotations17(name = "sampleConstructor", value = "Sample Constructor Annotation")
    public SampleClass17(String str) {

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
@interface CustomAnnotations17 {
    String name();

    String value();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n