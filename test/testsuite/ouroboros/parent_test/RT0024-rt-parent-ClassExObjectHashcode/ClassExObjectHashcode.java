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
 * -@TestCaseID: ClassExObjectHashcode.java
 * -@TestCaseName: Exception in Class: int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create Class ClassExObjectHashcode
 * -#step2: Create Object1 from ClassExObjectHashcode.class.getClass().
 * -#step3: Call hashCode of Object1, check no exception is thrown.
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ClassExObjectHashcode.java
 * -@ExecuteClass: ClassExObjectHashcode
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ClassExObjectHashcode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = classExObjectHashcode();
        } catch (Exception e) {
            ClassExObjectHashcode.res = ClassExObjectHashcode.res - 10;
        }
        if (result == 4 && ClassExObjectHashcode.res == 89) {
            result = 0;
        }

        return result;
    }

    private static int classExObjectHashcode() {
        // int hashCode()
        int result1 = 4; /*STATUS_FAILED*/
        Class<?> cal = ClassExObjectHashcode.class.getClass();
        try {
            int hashcode = cal.hashCode();
            ClassExObjectHashcode.res = ClassExObjectHashcode.res - 10;
        } catch (IllegalArgumentException e1) {
            ClassExObjectHashcode.res = ClassExObjectHashcode.res - 1;
        }
        return result1;
    }
}




// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n