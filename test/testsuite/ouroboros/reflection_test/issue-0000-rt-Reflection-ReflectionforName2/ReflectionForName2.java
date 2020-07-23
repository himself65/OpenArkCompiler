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
 * -@TestCaseID: ReflectionForName2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionForName2.java
 *- @Title/Destination: Use Class.forName to get class, Static initialization blocks for target classes are executed
 *                      when initialized set to True.
 *- @Brief:no:
 * -#step1: 定义一个类1，含有静态块，初始化会新建文件1。
 * -#step2：判断文件1是否存在，如果存在删除该文件。
 * -#step3：调用static Class<?> forName(String name, boolean initialize, ClassLoader loader)获取类1，initialize为true。
 * -#step4：检查类的静态块被初始化，产生新的文件1。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionForName2.java
 *- @ExecuteClass: ReflectionForName2
 *- @ExecuteArgs:
 */

import java.io.File;
import java.io.IOException;

class ForName2 {
    static {
        File cls2 = new File("/data/local/tmp/ReflectionForName2.txt");
        try {
            cls2.createNewFile();
        } catch (IOException e) {
            System.err.println(e);
        }
    }
}

public class ReflectionForName2 {
    public static void main(String[] args) throws ClassNotFoundException {
        File cls1 = new File("/data/local/tmp/ReflectionForName2.txt");
        if (cls1.exists()) {
            cls1.delete();
        }
        Class cls = Class.forName("ForName2", true, ForName2.class.getClassLoader());
        if (cls1.exists()) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n