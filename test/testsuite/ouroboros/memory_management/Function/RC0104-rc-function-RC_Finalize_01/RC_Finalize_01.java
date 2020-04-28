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
 * -@TestCaseID:maple/runtime/rc/function/RC_Finalize_01.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: test Finalizer for RC .
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_Finalize_01.java
 *- @ExecuteClass: RC_Finalize_01
 *- @ExecuteArgs:
 */
class C {
    static A a;
}

class A {
    B b;

    public A(B b) {

        this.b = b;

    }

    @Override
    public void finalize() throws Throwable {
        super.finalize();
//        System.out.println("A finalize");

        C.a = this;

    }

}


class B {
    int num1;
    int num2;

    public B(int num1, int num2) {
        this.num1 = num1;
        this.num2 = num2;
    }

    @Override
    public void finalize() throws Throwable {
        super.finalize();
//        System.out.println("B finalize");
    }

    public int sum() {
        return num1 + num2;
    }

}

public class RC_Finalize_01 {

    public static void main(String[] args) throws Exception {
        A a = new A(new B(12, 18));
        a = null;
        //System.gc();

        Thread.sleep(5000);
        System.out.println("ExpectResult");
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n