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
 * -@TestCaseID:String_intern.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:test Intern() API of String
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: stringIntern.java
 *- @ExecuteClass: stringIntern
 *- @ExecuteArgs:

 *
 */
public class stringIntern {
    private static int check = 0;

    public static void main(String[] args) {
        String s1 = new String("aaa");
        String s2 = "aaa";
        if (s1 != s2) {
            check++;    //=1
        }

        s1 = new String("bbb").intern();
        s2 = "bbb";
        if (s1 == s2) {
            check++;    //=2
        }

        s1 = "ccc";
        s2 = "ccc";
        if (s1 == s2) {
            check++;    //=3
        }

        s1 = new String("ddd").intern();
        s2 = new String("ddd").intern();
        if (s1 == s2) {
            check++;    //=4
        }

        s1 = "ab" + "cd";
        s2 = "abcd";
        if (s1 == s2) {
            check++;    //=5
        }

        String temp = "hh";
        s1 = "a" + temp;
        s2 = "ahh";
        if (s1 != s2) {
            check++;    //=6
        }

        temp = "hh".intern();
        s1 = "a" + temp;
        s2 = "ahh";
        if (s1 != s2) {
            check++;    //=7
        }

        temp = "hh".intern();
        s1 = ("a" + temp).intern();
        s2 = "ahh";
        if (s1 == s2) {
            check++;    //=8
        }

        s1 = new String("1");
        s1.intern();
        s2 = "1";
        if (s1 != s2) {
            check++;    //=9
        }

        String s3 = new String("1") + new String("1");
        s3.intern();
        String s4 = "11";
        if (s1 != s2) {
            check++;    //=10
        }

        s3 = new String("2") + new String("2");
        s4 = "22";
        s3.intern();
        if (s1 != s2) {
            check++;    //=11
        }
        if (check == 11) {
            System.out.println("ExpectResult");
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n