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
 * -@TestCaseID:maple/runtime/rc/function/CondBasedRC01.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: test condbasedrc for RC, decref can be omitted if ref is null can be guaranteed
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: CondBasedRC01.java
 *- @ExecuteClass: CondBasedRC01
 *- @ExecuteArgs:
 */
public class CondBasedRC01 {
    private String str1;
    public static String str2;

    public CondBasedRC01() {
    }

    public String testfunc(String s) {
        if (s == null) {
            s = str1;
        }
        return s;
    }

    public String strfunc() {
        return str1;
    }

    public String testfunc1() {
        String t = strfunc();
        if (t == null) {
            t = str1;
        }
        return t;
    }

    public static void main(String[] args) {
        CondBasedRC01 temp1 = new CondBasedRC01();
        String s = new String("test");
        str2 = temp1.testfunc(s);
        str2 = temp1.testfunc1();
        System.out.println("ExpectResult");
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n