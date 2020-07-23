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
 * -@TestCaseID:maple/runtime/rc/function/RC_basic.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: test RC basic operation.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_basic.java
 *- @ExecuteClass: RC_basic
 *- @ExecuteArgs:
 */
public class RC_basic {
    private Object obj;
    private String[] str;

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();
        System.out.println("ExpectResult");
    }

    public static void rc_testcase_main_wrapper() {
        RC_basic rc = new RC_basic();
        rc.obj = new Object();
        rc.str = new String[10];
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

    // simple new and recycle
    static void Test1() {
        RC_basic obj = new RC_basic();
        consume(obj);
    }

    // new and return
    static RC_basic Test2() {
        RC_basic obj = new RC_basic();
        return obj;
    }

    // new and consume and return
    static Object Test3() {
        RC_basic obj = new RC_basic();
        consume(obj);
        return obj;
    }

    // load and dec
    static void Test4(RC_basic rc) {
        Object obj = rc.obj;
        consume(obj);
    }

    // load and dec
    static void Test5(RC_basic rc) {
        consume(rc.obj);
    }

    // load and return
    static Object Test6(RC_basic rc) {
        return rc.obj;
    }

    // load consume and return
    static Object Test7(RC_basic rc) {
        Object o = rc.str;
        consume(o);
        return o;
    }


    // call and consume
    static void Test8() {
        RC_basic obj = Test2();
        consume(obj);
    }

    // call consume and return
    static Object Test9() {
        RC_basic obj = Test2();
        consume(obj);
        return obj;
    }

    // call and not consume
    static void Test10() {
        Test2();
    }

    public static void consume(Object obj) {
        //System.out.println("do noting");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n