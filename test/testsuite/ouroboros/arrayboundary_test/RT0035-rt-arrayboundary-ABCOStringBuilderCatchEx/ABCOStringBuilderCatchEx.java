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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOStringBuilderCatchEx.java
 * -@TestCaseName: StringIndexOutOfBoundsException is throw by StringBuilder toString
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new StringBuilder().
 * -#step2: call StringBuilder.toString().substring(), StringIndexOutOfBoundsException is throw
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOStringBuilderCatchEx.java
 * -@ExecuteClass: ABCOStringBuilderCatchEx
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

public class ABCOStringBuilderCatchEx {
    static int RES_PROCESS = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String argv[], PrintStream out) {
        int result = 4 /*STATUS_FAILED*/;
        try {
            result = test1();
        } catch (Exception e) {
            RES_PROCESS -= 10;
        }

        if (result == 1 && RES_PROCESS == 99) {
            result = 0;
        }
        return result;
    }

    public static int test1() {
        int res = 2 /*STATUS_FAILED*/;
        try {
            func();
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        } catch (Exception e) {
            res = res + 2;
        }
        return res;
    }

    public static void func() throws Exception {
        StringBuilder sbu = new StringBuilder();
        List<String> lii = new ArrayList<>();
        lii.add("welcome");
        lii.add("to");
        lii.add("hang");
        lii.add("zhou");
        lii.add("huawei");
        lii.add("company");
        for (String str : lii) {
            sbu.append(str).append(",");
        }
        int endIndex = (sbu.toString()).length() + 1;
        String getStr = sbu.toString().substring(0, endIndex);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n