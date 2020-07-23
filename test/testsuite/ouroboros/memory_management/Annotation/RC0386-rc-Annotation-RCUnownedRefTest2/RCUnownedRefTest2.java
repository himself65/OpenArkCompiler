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
 * -@TestCaseID:rc/unownedRef/RCUnownedRefTest2.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Test the basic function of @Unowned
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: RCUnownedRefTest2.java
 *- @ExecuteClass: RCUnownedRefTest2
 *- @ExecuteArgs:
 */

import com.huawei.ark.annotation.Unowned;

class Test_B {
    @Unowned
    Test_B bself;

    protected void run() {
        System.out.println("ExpectResult");
    }
}

class Test_A {

    Test_B bb;

    Test_B bb2;

    public void test() {
        foo();
        bb.bself.run();
        bb2.bself.run();
    }

    private void foo() {
        bb = new Test_B();
        bb2 = new Test_B();
        bb.bself = bb;
        bb2.bself = bb;
    }
}

public class RCUnownedRefTest2 {
    public static void main(String[] args) {
        new Test_A().test();
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n