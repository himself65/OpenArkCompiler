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
 * -@TestCaseID: ConstructorExAccessibleObjectsetAccessibleSecurityException.java
 * -@TestCaseName: Exception in reflect /Constructor:  void setAccessible(boolean flag)/ static void setAccessible(AccessibleObject[] array, boolean flag)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class SampleClass8, which has a single String parameter constructor, and annotate the class
 * -#step2: Call the run of the use case, and execute method1. In the method, get the constructor by calling getConstructor
 * -#step3: Call setAccessible(boolean flag) on object 1 to confirm isAccessible() that the returned is correct
 * -#step4: Call the run of the use case, and execute method2. In the method, get the constructor by calling getConstructors
 * -#step5: Call setAccessible(boolean flag) on object2[0], confirm to isAccessible() that the returned is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ConstructorExAccessibleObjectsetAccessibleSecurityException.java
 * -@ExecuteClass: ConstructorExAccessibleObjectsetAccessibleSecurityException
 * -@ExecuteArgs:
 */

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.AccessibleObject;

public class ConstructorExAccessibleObjectsetAccessibleSecurityException {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(run());
    }

    static int run() {
        int result = 2;
        result = constructorExAccessibleObjectsetAccessibleSecurityExceptionTest();
        constructorExAccessibleObjectsetAccessibleSecurityExceptionTest2();
        if (result == 4 && ConstructorExAccessibleObjectsetAccessibleSecurityException.res == 95) {
            result = 0;
        }
        return result;
    }

    private static int constructorExAccessibleObjectsetAccessibleSecurityExceptionTest() {
        int result1 = 4;

        try {
            // void setAccessible(boolean flag)
            AccessibleObject sampleconstructor = SampleClass8.class.getConstructor(String.class);
            sampleconstructor.setAccessible(true);
            boolean value = sampleconstructor.isAccessible();
            if (value) {
                // System.out.println(b);
                ConstructorExAccessibleObjectsetAccessibleSecurityException.res = ConstructorExAccessibleObjectsetAccessibleSecurityException.res - 2;
            }
        } catch (NoSuchMethodException | SecurityException e) {
            String eMsg = e.toString();
        }
        return result1;
    }


    private static void constructorExAccessibleObjectsetAccessibleSecurityExceptionTest2() {
        try {
            // static void setAccessible(AccessibleObject[] array, boolean flag)
            AccessibleObject[] sampleconstructorlist = SampleClass8.class.getConstructors();
            AccessibleObject.setAccessible(sampleconstructorlist, true);
            boolean value = sampleconstructorlist[0].isAccessible();
            if (value) {
                ConstructorExAccessibleObjectsetAccessibleSecurityException.res = ConstructorExAccessibleObjectsetAccessibleSecurityException.res - 2;
            }
        } catch (SecurityException e) {
            String eMsg = e.toString();
        }
    }
}

@CustomAnnotations8(name = "SampleClass", value = "Sample Class Annotation")
class SampleClass8 {
    private String sampleField;

    @CustomAnnotations8(name = "sampleConstructor", value = "Sample Constructor Annotation")
    public SampleClass8(String str) {

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
@interface CustomAnnotations8 {
    String name();

    String value();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n