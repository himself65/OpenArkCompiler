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
 * -@TestCaseID:maple/runtime/rc/function/RC_multest.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: test RC basic operation in For flow.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_multest.java
 *- @ExecuteClass: RC_multest
 *- @ExecuteArgs:
 */
public class RC_multest {
    static Foo f;
    private Object obj;
    private byte[] b;

    {
        f = new Bar();
    }

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
        System.out.println("ExpectResult");
    }

    private static void rc_testcase_main_wrapper() {
        TestSimple();
        TestParam();
        TestFlow();
        f = null;
    }

    static void TestSimple() {
        RC_multest rc = new RC_multest();
        rc.obj = new Object();
        rc.b = new byte[10];
        Test1();
        Test2();
        Test3();
        Test4(rc);
        Test5(rc);
        Test6(rc);
        Test7(rc);
        Test8();
        Test9();
        Test10();
    }

    static void TestParam() {
        RC_multest rc = new RC_multest();
        rc.obj = new Object();
        rc.b = new byte[10];
        TestParam1(rc);
        TestParam2(rc);
        TestParam3(rc);
    }

    static void TestFlow() {
        TestFlow1(21);
        TestFlow2(30);
    }

    // simple new and recycle
    static void Test1() {
        RC_multest obj = new RC_multest();
        consume(obj);
    }

    // new and return
    static RC_multest Test2() {
        RC_multest obj = new RC_multest();
        return obj;
    }

    // new and consume and return
    static Object Test3() {
        RC_multest obj = new RC_multest();
        consume(obj);
        return obj;
    }

    // load and dec
    static void Test4(RC_multest rc) {
        Object obj = rc.obj;
        consume(obj);
    }

    // load and dec
    static void Test5(RC_multest rc) {
        consume(rc.obj);
    }

    // load and return
    static Object Test6(RC_multest rc) {
        return rc.obj;
    }

    // load consume and return
    static Object Test7(RC_multest rc) {
        Object o = rc.b;
        consume(o);
        return o;
    }

    // call and consume
    static void Test8() {
        RC_multest obj = Test2();
        consume(obj);
    }

    // call consume and return
    static Object Test9() {
        RC_multest obj = Test2();
        consume(obj);
        return obj;
    }

    // call and not consume
    static void Test10() {
        Test2();
    }

    // parameter related
    // 1. parameter is returned
    // 2. parameter is redefined
    // 3. parameter is redefined conditionally
    static Object TestParam1(RC_multest rc) {
        consume(rc);
        return rc;
    }

    static Object TestParam2(RC_multest rc) {
        consume(rc);
        rc = Test2();
        return rc;
    }

    static Object TestParam3(RC_multest rc) {
        consume(rc);
        if (rc.b != null) {
            rc = Test2();
        }
        return rc;
    }

    static Object TestParam4() {
        RC_multest rc = new RC_multest();
        consume_return(rc);
        return rc;
    }

    // object field store

    // local variable
    // 1. local varaible is redefined in if/loop
    static Object TestFlow1(int num) {
        Object rc = null;
        for (int i = 1; i < num; i++) {
            if ((i % 5) == 0) {
                rc = new RC_multest();
            }
        }
        return rc;
    }

    static Object TestFlow2(int num) {
        Object rc = null;
        for (int i = 1; i < num; i++) {
            if ((i % 5) == 0) {
                rc = Test2();
            }
        }
        return rc;
    }

    // class info
    // exception
    static Object consume_return(Object obj) {
        return obj;
    }

    static void consume(Object obj) {
        f.consume(obj);
    }
}

class Foo {
    void consume(Object obj) {
    }
}

class Bar extends Foo {
    void consume(Object obj) {
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n