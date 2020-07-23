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
 * -@TestCaseID: MethodExAccessibleObjectSetAccessible.java
 * -@TestCaseName: Exception in reflect Method:   static void setAccessible(AccessibleObject[] array, boolean flag)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class MethodClass14, which has sampleMethod method, and annotate the method
 * -#step2: Call the run of the use case, and execute method 1. In the method, get the method of MethodClass14 by
 *          calling getMethod as Object 1
 * -#step3: Call setAccessible(AccessibleObject[] array, boolean flag) on object 1 to confirm that the returned are correct
 * -#step4: Call the run of the use case, and execute method 2. In the method, get the method of MethodClass14 by
 *          calling getMethod as Object 2
 * -#step5: Call setAccessible(boolean flag) on object 2 to confirm that the returned are correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: MethodExAccessibleObjectSetAccessible.java
 * -@ExecuteClass: MethodExAccessibleObjectSetAccessible
 * -@ExecuteArgs:
 */


import java.lang.reflect.AccessibleObject;
import java.lang.reflect.Method;


public class MethodExAccessibleObjectSetAccessible {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(run());
    }

    /**
     * main test fun
     * @return status code
     */
    public static int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = methodExAccessibleObjectSetAccessible1();
        } catch (Exception e) {
            MethodExAccessibleObjectSetAccessible.res = MethodExAccessibleObjectSetAccessible.res - 10;
        }

        try {
            result = methodExAccessibleObjectSetAccessible2();
        } catch (Exception e) {
            MethodExAccessibleObjectSetAccessible.res = MethodExAccessibleObjectSetAccessible.res - 10;
        }


        if (result == 4 && MethodExAccessibleObjectSetAccessible.res == 79) {
            result = 0;
        }
        return result;
    }

    private static int methodExAccessibleObjectSetAccessible1() throws NoSuchFieldException {
        int result1 = 4; /*STATUS_FAILED*/
        // SecurityException - Exception safety
        // static void setAccessible(AccessibleObject[] array, boolean flag)
        Method[] sampleMethod = MethodClass14.class.getMethods();

        try {
            AccessibleObject.setAccessible(sampleMethod, false);
            MethodExAccessibleObjectSetAccessible.res = MethodExAccessibleObjectSetAccessible.res - 10;
        } catch (SecurityException e1) {
            MethodExAccessibleObjectSetAccessible.res = MethodExAccessibleObjectSetAccessible.res - 1;
        }
        return result1;
    }

    private static int methodExAccessibleObjectSetAccessible2() throws NoSuchMethodException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/
        // SecurityException - Exception safety
        //  void setAccessible(boolean flag)

        Method sampleMethod = MethodClass14.class.getMethod("sampleMethod");
        try {
            sampleMethod.setAccessible(false);
            MethodExAccessibleObjectSetAccessible.res = MethodExAccessibleObjectSetAccessible.res - 10;
        } catch (SecurityException e1) {
            MethodExAccessibleObjectSetAccessible.res = MethodExAccessibleObjectSetAccessible.res - 1;
        }
        return result1;
    }
}

@CustomAnnotationwe(name = "SampleClass", value = "Sample Class Annotation")
class MethodClass14 {
    private String sampleField;

    @CustomAnnotationwe(name = "sampleMethod", value = "Sample Method Annotation")
    public String sampleMethod() {
        return "sample";
    }

    public String getSampleField() {
        return sampleField;
    }

    public void setSampleField(String sampleField) {
        this.sampleField = sampleField;
    }
}

@interface CustomAnnotationwe {
    String name();

    String value();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n