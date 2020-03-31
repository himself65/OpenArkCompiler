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
class Test_A {
    Test_B bb;
    void genCycle() {
        Test_B b = new Test_B();
        bb = b;
        b.aa = this;
    }
}

class Test_B {
    Test_A aa;
}

public class RCCycleTest {
    public static void main (String []args) {
        Test_A a = new Test_A();
        a.genCycle();
        if (a.bb == null) {
	        System.out.println("class B is collected");
        } else {
	        System.out.println("class B is not collected");
        }
    }
}
