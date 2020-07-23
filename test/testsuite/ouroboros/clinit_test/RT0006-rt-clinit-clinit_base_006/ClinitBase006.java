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
 * -@TestCaseID: ClinitBase006
 * -@TestCaseName: ClinitBase006.java
 * -@RequirementName: Java class initialization
 * -@Title/Destination: 首次为使用反射方式访问类的方法、字段、构造器，触发类初始化
 * -@Brief: 首次为使用反射方式访问类的方法、字段、构造器，触发类初始化
 *  -#step1: 调用forName不触发类初始化
 *  -#step2: 调用getDeclaredField触发类初始化
 *  -#step3: 调用newInstance触发类初始化
 *  -#step4:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClinitBase006.java
 * -@ExecuteClass: ClinitBase006
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

class CaseForName1 {
    static int field;

    static {
        if (ClinitBase006.getCount() == 1) {
            field = ClinitBase006.getCount();
        }
    }
}

class CaseForName2 {
    static int field;

    static {
        if (ClinitBase006.getCount() == 3) {
            field = ClinitBase006.getCount();
        }
    }
}

class CaseForName3 {
    static int field;

    static {
        if (ClinitBase006.getCount() == 5) {
            field = ClinitBase006.getCount();
        }
    }
}

class CaseClassInstance {
    static int field;

    static {
        if (ClinitBase006.getCount() == 7) {
            field = ClinitBase006.getCount();
        }
    }
}

class CaseClassMethod {
    static int field;

    static {
        if (ClinitBase006.getCount() == 5) {
            field = ClinitBase006.getCount();
        }
        System.out.println("hello");
    }

    static int getField() {
        return field;
    }
}

public class ClinitBase006 {

    public static int count;

    public static void main(String[] argv) {
        ClinitBase006 test = new ClinitBase006();
        System.out.println(test.run(argv, System.out)/*STATUS_TEMP*/);
    }

    static int getCount() {
        count += 1;
        return count;
    }

    private int run(String[] argv, PrintStream out) {
        int res = 0;

        try {
            Class forName1 = Class.forName("CaseForName1");
            if (count != 2 || forName1.getDeclaredField("field").getInt(null) != 2) {
                res = 2;
                out.println("Error, CaseForName1 not initialized");
            }

            Class forName2 = Class.forName("CaseForName2", true, this.getClass().getClassLoader());
            if (count != 4 || forName2.getDeclaredField("field").getInt(null) != 4) {
                res = 2;
                out.println("Error, CaseForName2 not initialized");
            }

            Class forName3 = Class.forName("CaseForName3", false, this.getClass().getClassLoader());
            if (count != 4 || forName3.getDeclaredField("field").getInt(null) != 6) {
                res = 2;
                out.println("Error, CaseForName3 not initialized");
            }

            CaseClassInstance caseInstance = CaseClassInstance.class.newInstance();
            if (count != 8 || CaseClassInstance.field != 8) {
                res = 2;
                out.println("Error, case6_instance not initialized");
            }
        } catch (Exception e) {
            res = 2;
            out.println(e);
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n