
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
 * -@TestCaseID:maple/runtime/rc/function/RC_newObjectIassign_01.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: 针对RC优化new object assign 做场景测试：对象类中的某个域如果在类初始化时没有做赋值，则不会对它的incref(无集成场景)
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_newObjectIassign_01.java
 *- @ExecuteClass: RC_newObjectIassign_01
 *- @ExecuteArgs:
 */

import java.util.Arrays;

public class RC_newObjectIassign_01 {
    final static String STR = "MapleTest";
    static int check = 0;
    String[] strArray;
    static String str1;
    volatile String str2;

    private RC_newObjectIassign_01() {
        strArray = new String[10];
        str1 = "MapleTest";
        str2 = "FigoTest";
    }

    private RC_newObjectIassign_01(String str) {
        str1 = str;
        str2 = str;
    }

    private RC_newObjectIassign_01(String[] str) {
        strArray = str;
    }

    public static void main(String[] args) {
        for (int i = 1; i <= 100; i++) {
            test_new_objct_assign();
            if (check != 5) {
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
        String[] arrayStr = {"a", "b", "c"};
        RC_newObjectIassign_01 rctest = new RC_newObjectIassign_01(arrayStr);
        if (Arrays.equals(rctest.strArray, example) && rctest.str1 == null && rctest.str2 == null)
            check += 1;
        rctest.str1 = "firstTimeTestStr1";
        rctest.str2 = "firstTimeTestStr2";
        if (Arrays.equals(rctest.strArray, example) && rctest.str1 == "firstTimeTestStr1" && rctest.str2 == "firstTimeTestStr2")
            check += 1;

        rctest = new RC_newObjectIassign_01("secondTimeTest");
        if (Arrays.equals(rctest.strArray, null) && rctest.str1 == "secondTimeTest" && rctest.str2 == "secondTimeTest")
            check += 1;
        rctest.strArray = arrayStr;
        if (Arrays.equals(rctest.strArray, example) && rctest.str1 == "secondTimeTest" && rctest.str2 == "secondTimeTest")
            check += 1;

        rctest = new RC_newObjectIassign_01();
        if (rctest.strArray.length == 10 && rctest.str1 == "MapleTest" && rctest.str2 == "FigoTest")
            check += 1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n