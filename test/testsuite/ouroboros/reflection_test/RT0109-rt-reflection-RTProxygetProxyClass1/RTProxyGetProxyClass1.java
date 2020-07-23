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
 * -@TestCaseID: RTProxyGetProxyClass1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTProxyGetProxyClass1.java
 *- @Title/Destination: Verify that there are more than 30 methods in the class that perform the proxy, and you can
 *                     construct the proxy function normally.
 *- @Brief:no:
 * -#step1: 构造接口1有30个default方法，接口2继承接口1，接口3有1个方法。
 * -#step2：创建参数1loader为接口1的类加载器，参数2interfaces为class数组，成员为接口1的class,接口2的class。
 * -#step3：调用Proxy的静态方法getProxyClass(ClassLoader loader, Class<?>... interfaces)。
 * -#step4：确认无异常抛出。
 * -#step5：创建参数1loader为接口1的类加载器，参数2interfaces为class数组，成员为接口1的class,接口3的class。
 * -#step6：调用Proxy的静态方法getProxyClass(ClassLoader loader, Class<?>... interfaces)。
 * -#step7：确认无异常抛出。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTProxyGetProxyClass1.java
 *- @ExecuteClass: RTProxyGetProxyClass1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Proxy;

interface ProxyGetProxyClass1_1 {
    default void test1() {
    }

    default void test2() {
    }

    default void test3() {
    }

    default void test4() {
    }

    default void test5() {
    }

    default void test6() {
    }

    default void test7() {
    }

    default void test8() {
    }

    default void test9() {
    }

    default void test10() {
    }

    default void test11() {
    }

    default void test12() {
    }

    default void test13() {
    }

    default void test14() {
    }

    default void test15() {
    }

    default void test16() {
    }

    default void test17() {
    }

    default void test18() {
    }

    default void test19() {
    }

    default void test20() {
    }

    default void test21() {
    }

    default void test22() {
    }

    default void test23() {
    }

    default void test24() {
    }

    default void test25() {
    }

    default void test26() {
    }

    default void test27() {
    }

    default void test28() {
    }

    default void test29() {
    }

    default void test30() {
    }
}

interface ProxyGetProxyClass1_2 extends ProxyGetProxyClass1_1 {
    default void test30() {
    }
}

interface ProxyGetProxyClass1_3 {
    default void test31() {
    }
}

public class RTProxyGetProxyClass1 {
    public static void main(String[] args) {
        try {
            Proxy.getProxyClass(ProxyGetProxyClass1_1.class.getClassLoader(), new Class[]{ProxyGetProxyClass1_1.class,
                    ProxyGetProxyClass1_2.class});
            Proxy.getProxyClass(ProxyGetProxyClass1_1.class.getClassLoader(), new Class[]{ProxyGetProxyClass1_1.class,
                    ProxyGetProxyClass1_3.class});
        } catch (IllegalArgumentException e) {
            System.err.println(e);
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n