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
 * -@TestCaseID: AccessibleObjectExObjecthashCode.java
 * -@TestCaseName: Exception in reflect/AccessibleObject: int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create Class SampleClass Field with sampleField0, sampleField1
 * -#step2: Create Object1 Get the Field of the sampleField0 of SampleClass through the getDeclaredField method
 * -#step3: Create Object2 Get the Field of sampleField0 of SampleClass by getDeclaredField method
 * -#step4: Create Object3 Get the Field of sampleField1 of SampleClass by getDeclaredField method
 * -#step5: Call hashCode to compare, object1 and object2 confirm that they are equal, object2 and object3 confirm that
 *          they are not equal
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: AccessibleObjectExObjecthashCode.java
 * -@ExecuteClass: AccessibleObjectExObjecthashCode
 * -@ExecuteArgs:
 */

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.reflect.AccessibleObject;

public class AccessibleObjectExObjecthashCode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new AccessibleObjectExObjecthashCode().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = accessibleObjectExObjecthashCode1();
        } catch (Exception e) {
            AccessibleObjectExObjecthashCode.res = AccessibleObjectExObjecthashCode.res - 20;
        }

        if (result == 4 && AccessibleObjectExObjecthashCode.res == 89) {
            result = 0;
        }
        return result;
    }

    private int accessibleObjectExObjecthashCode1() throws NoSuchFieldException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/
        // int hashCode()
        AccessibleObject sampleField0 = SampleClass.class.getDeclaredField("sampleField0");
        AccessibleObject sampleField1 = SampleClass.class.getDeclaredField("sampleField0");
        AccessibleObject sampleField2 = SampleClass.class.getDeclaredField("sampleField1");
        int px0 = sampleField0.hashCode();
        int px1 = sampleField1.hashCode();
        int px2 = sampleField2.hashCode();
        if (px0 == px1 && px0 != px2) {
            AccessibleObjectExObjecthashCode.res = AccessibleObjectExObjecthashCode.res - 10;
        }
        return result1;
    }
}


@CustomAnnotation(name = "SampleClass", value = "Sample Class Annotation")
class SampleClass {
    private String sampleField0;

    private String sampleField1;

    @CustomAnnotation(name = "sampleMethod", value = "Sample Method Annotation")
    public String sampleMethod() {
        return "sample";
    }

    /**
     * get sampleField0
     * @return sampleField0
     */
    public String getSampleField() {
        return sampleField0;
    }

    /**
     * set sampleField0
     * @param sampleField just for test
     */
    public void setSampleField(String sampleField) {
        this.sampleField0 = sampleField;
    }
}

@Retention(RetentionPolicy.RUNTIME)
@interface CustomAnnotation {
    String name();

    String value();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n