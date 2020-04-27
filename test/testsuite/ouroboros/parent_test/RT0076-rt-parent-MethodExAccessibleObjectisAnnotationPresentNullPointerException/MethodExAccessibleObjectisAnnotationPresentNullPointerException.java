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
 * -@TestCaseID: MethodExAccessibleObjectisAnnotationPresentNullPointerException.java
 * -@TestCaseName: Exception in reflect Method:public  boolean isAnnotationPresent(Class<? extends Annotation> annotationClass)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class MethodClass8, which has sampleMethod method, and annotate the method
 * -#step2: Call the run of the use case, and execute method 1. In the method, get the method of MethodClass8 by
 *          calling getMethod as Object 1
 * -#step3: Call isAnnotationPresent(Class<? extends Annotation> annotationClass) on object 1 to confirm that the returned are correct
 * -#step4: execute method 2. In the method, get the method of SampleClass_h6 by calling getMethod as Object 2
 * -#step5: Call isAnnotationPresent(null) on object 2 with null parameter, confirm to throw NullPointerException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: MethodExAccessibleObjectisAnnotationPresentNullPointerException.java
 * -@ExecuteClass: MethodExAccessibleObjectisAnnotationPresentNullPointerException
 * -@ExecuteArgs:
 */


import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.reflect.Method;


public class MethodExAccessibleObjectisAnnotationPresentNullPointerException {
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
            result = methodExAccessibleObjectisAnnotationPresentNullPointerException1();
        } catch (Exception e) {
            MethodExAccessibleObjectisAnnotationPresentNullPointerException.res = MethodExAccessibleObjectisAnnotationPresentNullPointerException.res - 20;
        }

        try {
            result = methodExAccessibleObjectisAnnotationPresentNullPointerException2();
        } catch (Exception e) {
            MethodExAccessibleObjectisAnnotationPresentNullPointerException.res = MethodExAccessibleObjectisAnnotationPresentNullPointerException.res - 20;
        }


        if (result == 4 && MethodExAccessibleObjectisAnnotationPresentNullPointerException.res == 68) {
            result = 0;
        }
        return result;
    }

    private static int methodExAccessibleObjectisAnnotationPresentNullPointerException1() throws NoSuchMethodException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/
        // NullPointerException - If the specified object is empty.
        Method sampleMethod = MethodClass8.class.getMethod("sampleMethod");
        try {
            sampleMethod.isAnnotationPresent(null);
            MethodExAccessibleObjectisAnnotationPresentNullPointerException.res = MethodExAccessibleObjectisAnnotationPresentNullPointerException.res - 10;
        } catch (NullPointerException e1) {
            MethodExAccessibleObjectisAnnotationPresentNullPointerException.res = MethodExAccessibleObjectisAnnotationPresentNullPointerException.res - 1;
        }
        return result1;
    }

    private static int methodExAccessibleObjectisAnnotationPresentNullPointerException2() throws NoSuchMethodException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/
        // NullPointerException - If the specified object is empty.
        // boolean isAnnotationPresent(Class<? extends Annotation> annotationClass)
        Method sampleMethod = MethodClass8.class.getMethod("sampleMethod");
        try {
            sampleMethod.isAnnotationPresent(CustomAnnotationH100.class);
            MethodExAccessibleObjectisAnnotationPresentNullPointerException.res = MethodExAccessibleObjectisAnnotationPresentNullPointerException.res - 30;
        } catch (NullPointerException e1) {
            MethodExAccessibleObjectisAnnotationPresentNullPointerException.res = MethodExAccessibleObjectisAnnotationPresentNullPointerException.res - 5;
        }
        return result1;
    }

}

@CustomAnnotationH100(name = "SampleClass", value = "Sample Class Annotation")
class MethodClass8 {
    private String sampleField;

    @CustomAnnotationH100(name = "sampleMethod", value = "Sample Method Annotation")
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

@Retention(RetentionPolicy.RUNTIME)
@interface CustomAnnotationH100 {
    String name();

    String value();
}




// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n