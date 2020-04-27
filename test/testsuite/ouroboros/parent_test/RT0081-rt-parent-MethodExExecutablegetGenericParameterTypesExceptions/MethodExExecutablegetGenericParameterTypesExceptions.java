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
 * -@TestCaseID: MethodExExecutablegetGenericParameterTypesExceptions.java
 * -@TestCaseName: Exception in reflect/Method:Type[] getGenericParameterTypes()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class SampleClass_h3, which has getSampleField method, and annotate the method
 * -#step2: Call the run of the use case, and execute method 1. In the method, get the method of SampleClass_h3 by
 *          calling getMethod as Object 1
 * -#step3: Call getGenericParameterTypes() on object 1 to confirm that the returned are correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: MethodExExecutablegetGenericParameterTypesExceptions.java
 * -@ExecuteClass: MethodExExecutablegetGenericParameterTypesExceptions
 * -@ExecuteArgs:
 */


import java.lang.reflect.Method;
import java.lang.reflect.Type;
import java.util.Arrays;


public class MethodExExecutablegetGenericParameterTypesExceptions {
    static int res = 99;

    public static void main(String[] argv) {
        System.out.println(new MethodExExecutablegetGenericParameterTypesExceptions().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = methodExExecutablegetGenericParameterTypesExceptions1();
        } catch (Exception e) {
            MethodExExecutablegetGenericParameterTypesExceptions.res = MethodExExecutablegetGenericParameterTypesExceptions.res - 20;
        }

        if (result == 4 && MethodExExecutablegetGenericParameterTypesExceptions.res == 89) {
            result = 0;
        }

        return result;
    }

    private int methodExExecutablegetGenericParameterTypesExceptions1() throws NoSuchMethodException {
        //  Type[] getGenericParameterTypes()
        int result1 = 4;
        Method m1 = SampleClassH3.class.getDeclaredMethod("getSampleField");
        try {
            Type[] tp = m1.getGenericParameterTypes();

            if (Arrays.toString(tp).equals("[]")) {
                MethodExExecutablegetGenericParameterTypesExceptions.res = MethodExExecutablegetGenericParameterTypesExceptions.res - 10;
            }
        } catch (Exception e) {
            MethodExExecutablegetGenericParameterTypesExceptions.res = MethodExExecutablegetGenericParameterTypesExceptions.res - 15;
        }

        return result1;
    }
}

@CustomAnnotationsH8(name = "SampleClass", value = "Sample Class Annotation")
class SampleClassH3 {
    private String sampleField;

    public String getSampleField() throws ArrayIndexOutOfBoundsException {
        return sampleField;
    }

    public void setSampleField(String sampleField) {
        this.sampleField = sampleField;
    }
}

@interface CustomAnnotationsH8 {
    String name();

    String value();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n