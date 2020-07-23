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
 * -@TestCaseID:maple/runtime/rc/function/PhantomRefTest.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: PhantomRefTest basic testcase
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: PhantomRefTest.java
 *- @ExecuteClass: PhantomRefTest
 *- @ExecuteArgs:
 *
 */

import java.lang.ref.*;

public class PhantomRefTest {
    static Reference rp;
    static ReferenceQueue rq = new ReferenceQueue();
    static int a = 100;

    static void setPhantomReference() {
        rp = new PhantomReference<Object>(new Object(), rq);
        if (rp.get() != null) {
            a++;
        }
    }

    public static void main(String[] args) throws Exception {
        Reference r;
        setPhantomReference();
        Thread.sleep(2000);
        while ((r = rq.poll()) != null) {
            if (!r.getClass().toString().equals("class java.lang.ref.PhantomReference")) {
                a++;
            }
        }

        if (a == 100) {
            System.out.println("ExpectResult");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n