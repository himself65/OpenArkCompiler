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
 * -@TestCaseID: RTFieldGet3
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldGet3.java
 *- @Title/Destination: If the filed is not given a value, the default value is returned by reflection。
 *- @Brief:no:
 * -#step1: 定义类FieldGet3_a，定义FieldGet3_a的子类FieldGet3。
 * -#step2: 通过调用forName()方法加载类FieldGet3，调用newInstance()生产实例对象。
 * -#step3: 通过调用getField()获取对应名称的未初始化的成员变量。
 * -#step4: 调用get(Object obj)方法作出判断，确认String类型为null,int类型为0。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldGet3.java
 *- @ExecuteClass: RTFieldGet3
 *- @ExecuteArgs:
 */

class FieldGet3_a {
    public int num;
}

class FieldGet3 extends FieldGet3_a {
    public static String str;
}

public class RTFieldGet3 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldGet3");
            Object obj = cls.newInstance();
            Object obj1 = cls.getField("str").get(obj);
            Object obj2 = cls.getField("num").get(obj);
            if (obj1 == null && (int) obj2 == 0) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (InstantiationException e2) {
            System.err.println(e2);
            System.out.println(2);
        } catch (NoSuchFieldException e3) {
            System.err.println(e3);
            System.out.println(2);
        } catch (IllegalAccessException e4) {
            System.err.println(e4);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n