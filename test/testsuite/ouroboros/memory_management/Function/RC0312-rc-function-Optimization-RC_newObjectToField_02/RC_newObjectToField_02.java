
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
 * -@TestCaseID:maple/runtime/rc/function/RC_newObjectToField_02.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: 对于像mR = new Obj（）的语句; 为了分配一个新对象并将其引用分配给一个field，我们正在生成一对冗余的RC Inc / Dec操作
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_newObjectToField_02.java
 *- @ExecuteClass: RC_newObjectToField_02
 *- @ExecuteArgs:
 */

import java.util.Arrays;

public class RC_newObjectToField_02 {
    final static String STR = "MapleTest";
    static int check = 0;
    volatile static String[] strArray;
    int[] intArray;
    static String str1;
    volatile String str2;

    private RC_newObjectToField_02() throws ArithmeticException {
        strArray = new String[10];
        str1 = new String("str1") + "MapleTest";
        str2 = "FigoTest";
        intArray = new int[]{1 / 0};
    }

    private RC_newObjectToField_02(String str) throws StringIndexOutOfBoundsException {
        str1 = (new String("str1") + str).substring(-1, -2);
        str2 = new String("str2") + str;
    }

    private RC_newObjectToField_02(String[] str) throws ArrayIndexOutOfBoundsException {
        Arrays.sort(str);
        Arrays.toString(str);
        new Object();
        new String("Just Test");
        Arrays.binarySearch(str, "d");
        intArray = new int[10];
        strArray = new String[1];
        strArray[2] = new String("IndexOutBounds");
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
        String[] arrayStr = {"c", "a", "b"};
        try {
            RC_newObjectToField_02 rctest = new RC_newObjectToField_02(arrayStr);
        } catch (ArrayIndexOutOfBoundsException a) {
            check += 1;
        }
        try {
            RC_newObjectToField_02 rctest = new RC_newObjectToField_02("secondTimeTest");
        } catch (StringIndexOutOfBoundsException s) {
            check += 1;
        }
        try {
            RC_newObjectToField_02 rctest = new RC_newObjectToField_02();
        } catch (ArithmeticException a) {
            check += 1;
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n
