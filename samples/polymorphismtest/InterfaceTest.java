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
 */
interface Inter {
    public default void foo() {
        System.out.println("Inter.foo()");
    }
}

class Base implements Inter {
    public void foo() {
        System.out.println("Base.foo()");
    }
}

class Derived extends Base {
    public void foo() {
        System.out.println("Derived.foo()");
    }
}

public class InterfaceTest {
    public static void main(String[] args) {
        Derived o1 = new Derived();
        o1.foo();
        Base o2 = new Derived();
        o2.foo();
        Inter o3 = new Derived();
        o3.foo();
    }
}
