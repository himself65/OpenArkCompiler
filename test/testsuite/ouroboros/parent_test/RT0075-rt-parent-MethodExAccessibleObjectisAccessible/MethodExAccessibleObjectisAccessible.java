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
 * -@TestCaseID: MethodExAccessibleObjectisAccessible.java
 * -@TestCaseName: Exception in reflect Method:public   boolean isAccessible()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class MethodClass11, which has sampleMethod method, and annotate the method
 * -#step2: Call the run of the use case, and execute method 1. In the method, get the method of MethodClass11 by
 *          calling getMethod as Object 1
 * -#step3: Call isAccessible() on object 1 to confirm that the returned are correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: MethodExAccessibleObjectisAccessible.java
 * -@ExecuteClass: MethodExAccessibleObjectisAccessible
 * -@ExecuteArgs:
 */

import java.lang.reflect.Method;


public class MethodExAccessibleObjectisAccessible {
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
            result = methodExAccessibleObjectisAccessible1();
        } catch (Exception e) {
            MethodExAccessibleObjectisAccessible.res = MethodExAccessibleObjectisAccessible.res - 10;
        }


        if (result == 4 && MethodExAccessibleObjectisAccessible.res == 98) {
            result = 0;
        }

        return result;
    }

    private static int methodExAccessibleObjectisAccessible1() throws NoSuchMethodException, SecurityException, NoSuchFieldException {
        int result1 = 4; /*STATUS_FAILED*/
        // boolean isAccessible()
        Method sampleMethod = MethodClass11.class.getMethod("sampleMethod");

        if (sampleMethod.isAccessible()) {
            MethodExAccessibleObjectisAccessible.res = MethodExAccessibleObjectisAccessible.res - 10;
        } else {
            MethodExAccessibleObjectisAccessible.res = MethodExAccessibleObjectisAccessible.res - 1;
        }

        return result1;
    }

}

@CustomAnnotationn(name = "SampleClass", value = "Sample Class Annotation")
class MethodClass11 {
    private String sampleField;

    @CustomAnnotationn(name = "sampleMethod", value = "Sample Class Annotation")
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

@interface CustomAnnotationn {
    String name();

    String value();
}




// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n