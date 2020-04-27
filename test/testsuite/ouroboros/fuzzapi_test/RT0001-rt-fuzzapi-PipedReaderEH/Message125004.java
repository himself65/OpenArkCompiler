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
 * -@TestCaseID: Message125004.java
 *- @Title/Destination: PipedReader.getField() should get correct unstack when EH
 *- @Brief:
 *- @Expect:expected.txt
 *- @Priority: High
 *- @Source: Message125004.java
 *- @ExecuteClass: Message125004
 *- @ExecuteArgs:
 */

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class Message125004 {
    private static int RES = 99;
    private static Class<?> clazz;

    public static void main(String[] args) {
        int result;
        try {
            result = message125004();
            if (result == 4 && Message125004.RES == 89) {
                result = 0;
            }
            System.out.println(result);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }



    /**测试实例：[{NeedRandomClass=java.io.PipedReader}]     测试入参：abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ257     测试api信息：java.lang.Class
     *  测试api信息：getField
     测试api信息：String
     */
    public static int message125004() {
        try {
            clazz = Class.forName("java.lang.Class");
            Method method = clazz.getMethod("getField",java.lang.String.class);
            String parameters1 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ257";
            Class instance = Class.forName("java.io.PipedReader") ;
            Object result1 = method.invoke(instance, parameters1);
            Message125004.RES -= 10;
            return 4;
            } catch (InvocationTargetException e) {
                // 只打印类型 由Agent端接受进行对比
                // 打印全错误信息
//                e.printStackTrace();
                Message125004.RES -= 10;
                return 4;
            } catch (Exception e) {
            // 只打印类型 由Agent端接受进行对比
            // 打印全错误信息
//            e.printStackTrace();
            Message125004.RES -= 10;
            return 4;
        }
    }


}



// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0
