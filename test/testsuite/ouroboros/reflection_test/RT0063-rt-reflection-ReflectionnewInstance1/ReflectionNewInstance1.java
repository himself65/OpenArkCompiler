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
 * -@TestCaseID: ReflectionNewInstance1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionNewInstance1.java
 *- @Title/Destination: Instantiating a class by reflection:class.newInstance() creates a new instance of the class
 *                      represented by this Class object. The class is instantiated as if by a new expression with an
 *                      empty argument list.
 *- @Brief:no:
 * -#step1: Use classloader load class.
 * -#step2: Instantiating the class by class.newInstance().
 * -#step3: Check that the object is  instance of class NewInstance1
 * -#step4: Try to catch exceptions those occurs possibly.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionNewInstance1.java
 *- @ExecuteClass: ReflectionNewInstance1
 *- @ExecuteArgs:
 */

class NewInstance1 {
}

public class ReflectionNewInstance1 {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp = Class.forName("NewInstance1");
            Object zhu = zqp.newInstance();
            if (zhu.toString().indexOf("NewInstance1@") != -1) {
                result = 0;
            }
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            result = -1;
        } catch (InstantiationException e1) {
            System.err.println(e1);
            result = -1;
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