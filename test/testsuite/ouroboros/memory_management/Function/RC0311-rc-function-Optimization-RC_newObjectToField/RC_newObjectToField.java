
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
 * -@TestCaseID:maple/runtime/rc/function/RC_newObjectToField.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: 对于像mR = new Obj（）的语句; 为了分配一个新对象并将其引用分配给一个field，我们正在生成一对冗余的RC Inc / Dec操作
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_newObjectToField.java
 *- @ExecuteClass: RC_newObjectToField
 *- @ExecuteArgs:
 */

import java.util.Arrays;

public class RC_newObjectToField {
    final static String STR = "MapleTest";
    static int check = 0;
    volatile static String[] strArray;
    static String str1;
    volatile String str2;

    private RC_newObjectToField() {
        strArray = new String[10];
        str1 = new String("str1") + "MapleTest";
        str2 = "FigoTest";
    }

    private RC_newObjectToField(String str) {
        str1 = new String("str1") + str;
        str2 = new String("str2") + str;
    }

    private RC_newObjectToField(String[] str) {
        Arrays.sort(str);
        Arrays.toString(str);
        new Object();
        new String("Just Test");
        Arrays.binarySearch(str, "d");
        strArray = new String[10];
        strArray = str;
    }


    public static void main(String[] args) {
        for (int i = 1; i <= 100; i++) {
            test_new_objct_assign();
            if (check != 3) {
                System.out.println("ErrorResult");
                break;
            } else {
                check = 0;
                str1 = null;
            }

        }
        System.out.println("ExpectResult");
    }

    private static void test_new_objct_assign() {
        String[] example = {"a", "b", "c"};
        String[] arrayStr = {"c", "a", "b"};
        RC_newObjectToField rctest = new RC_newObjectToField(arrayStr);
        if (Arrays.equals(rctest.strArray, example) && rctest.str1 == null && rctest.str2 == null)
            check += 1;

        rctest = new RC_newObjectToField("secondTimeTest");
        if (Arrays.equals(rctest.strArray, example) && rctest.str1.equals("str1secondTimeTest") && rctest.str2.equals("str2secondTimeTest"))
            check += 1;

        rctest = new RC_newObjectToField();
        if (rctest.strArray.length == 10 && rctest.str1.equals("str1MapleTest") && rctest.str2 == "FigoTest")
            check += 1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n