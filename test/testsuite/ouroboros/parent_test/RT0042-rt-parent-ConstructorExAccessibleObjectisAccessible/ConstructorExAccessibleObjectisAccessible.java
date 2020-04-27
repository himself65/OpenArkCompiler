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
 * -@TestCaseID: ConstructorExAccessibleObjectisAccessible.java
 * -@TestCaseName: Exception in reflect /Constructor:   boolean isAccessible()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class SampleClass6, which has a single String parameter constructor, and annotate the class with
 *          CustomAnnotations
 * -#step2: Call the run of the use case, and execute method1. In the method, get the constructor by calling getConstructor
 * -#step3: Call isAccessible() on object 1 to confirm that the returned is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ConstructorExAccessibleObjectisAccessible.java
 * -@ExecuteClass: ConstructorExAccessibleObjectisAccessible
 * -@ExecuteArgs:
 */


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.AccessibleObject;

public class ConstructorExAccessibleObjectisAccessible {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(run());
    }

    static int run() {
        int result = 2;
        try {
            result = constructorExAccessibleObjectisAccessibleTest();
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (result == 4 && ConstructorExAccessibleObjectisAccessible.res == 97) {
            result = 0;
        }
        return result;
    }

    private static int constructorExAccessibleObjectisAccessibleTest() {
        int result = 4;
        try {
            // boolean isAccessible()
            AccessibleObject sampleconstructor = SampleClass6.class.getConstructor(String.class);
            boolean value = sampleconstructor.isAccessible();
            // System.out.println(a);
            if (!value) {
                ConstructorExAccessibleObjectisAccessible.res = ConstructorExAccessibleObjectisAccessible.res - 2;
            }
        } catch (NoSuchMethodException | SecurityException | NullPointerException e) {
            String eMsg = e.toString();
        }
        return result;
    }
}

@CustomAnnotations6(name = "SampleClass", value = "Sample Class Annotation")
class SampleClass6 {
    private String sampleField;

    @CustomAnnotations6(name = "sampleConstructor", value = "Sample Constructor Annotation")
    public SampleClass6(String str) {
        this.sampleField = str;
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
@interface CustomAnnotations6 {
    String name();

    String value();
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n