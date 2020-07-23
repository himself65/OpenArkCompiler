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
 * -@TestCaseID: ReflectionNewInstanceInstantiationExceptionIllegalAccessException
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionNewInstanceInstantiationExceptionIllegalAccessException.java
 *- @Title/Destination: call newInstance() on an interface object throws InstantiationException; if the class's
 *                      constructor is not accessible, throws IllegalAccessException.
 *- @Brief:no:
 * -#step1: Create a test class.
 * -#step2: Use classloader to load class.
 * -#step3: Instantiating the class by class.newInstance().
 * -#step4: It will throw InstantiationException because we used class which dose not exit.
 * -#step5: Then check that IllegalAccessException was threw when class constructor is not accessible.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionNewInstanceInstantiationExceptionIllegalAccessException.java
 *- @ExecuteClass: ReflectionNewInstanceInstantiationExceptionIllegalAccessException
 *- @ExecuteArgs:
 */

interface NewInstance2 {
}

class NewInstance2_a {
    private NewInstance2_a() {
    }
}

public class ReflectionNewInstanceInstantiationExceptionIllegalAccessException {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp1 = Class.forName("NewInstance2");
            Object zhu1 = zqp1.newInstance();
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            result = -1;
        } catch (InstantiationException e1) {

            try {
                Class zqp2 = Class.forName("NewInstance2_a");
                Object zhu2 = zqp2.newInstance();
            } catch (ClassNotFoundException e3) {
                System.err.println(e3);
                result = -1;
            } catch (InstantiationException e4) {
                System.err.println(e4);
                result = -1;
            } catch (IllegalAccessException e5) {
                result = 0;
            }
        } catch (IllegalAccessException e2) {
            System.err.println(e2);
            result = -1;
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n