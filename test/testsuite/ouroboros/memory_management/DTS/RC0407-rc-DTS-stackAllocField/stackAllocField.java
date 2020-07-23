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
 * -@TestCaseID:stackAllocField.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:test Intern() API of String
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: stackAllocField.java
 *- @ExecuteClass: stackAllocField
 *- @ExecuteArgs:

 *
 */
public class stackAllocField {
    private static A a1 = new A();
    private static A a2 = new A();
    private static A a3 = new A();
    private static A a4 = new A();

    public static void main(String[] args) {
        wrapper(a1, a2, a3, a4);
    }

    public static void wrapper(A tempa1, A tempa2, A tempa3, A tempa4) {
        A testa1 = new A();
        testa1.a = tempa1;
        A testa2 = new A();
        testa2.a = tempa2;
        A testa3 = new A();
        testa3.a = tempa3;
        A testa4 = new A();
        testa4.a = tempa4;
        tempa1 = null;
        tempa2 = null;
        tempa3 = null;
        tempa4 = null;
        a1 = null;
        a2 = null;
        a3 = null;
        a4 = null;
        for (int i = 0; i <= 50; i++) {
            Runtime.getRuntime().gc();
        }
        if (testa1.a != null && testa2.a != null && testa3.a != null && testa4.a != null) {
            System.out.println("ExpectResult");
        }
    }

    private static class A {
        A a;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n