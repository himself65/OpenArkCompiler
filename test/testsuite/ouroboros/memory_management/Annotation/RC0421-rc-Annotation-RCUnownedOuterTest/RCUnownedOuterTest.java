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
 * -@TestCaseID:maple/runtime/rc/annotation/RCUnownedOuterTest
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:在内部类和内部匿名类添加@UnownedOuter,没有环泄露
 *- @Brief:functionTest
 * 在内部匿名类和内部类添加@UnownedOuter,没有环泄露
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RCUnownedOuterTest.java
 *- @ExecuteClass: RCUnownedOuterTest
 *- @ExecuteArgs:
 */

import com.huawei.ark.annotation.UnownedOuter;

import java.util.LinkedList;
import java.util.List;

public class RCUnownedOuterTest {


    public static void main(String[] args) {
        UnownedAnnoymous unownedAnnoymous = new UnownedAnnoymous();
        unownedAnnoymous.anonymousCapture();

        UnownedInner unownedInner = new UnownedInner();
        unownedInner.method();
        UnownedInner.InnerClass innerClass = unownedInner.new InnerClass();
        innerClass.myName();
        if (unownedAnnoymous.checkAnnoy == 1 && unownedInner.checkInner == 1) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("error");
            System.out.println("unownedAnnoymous.checkAnnoy:" + unownedAnnoymous.checkAnnoy);
            System.out.println("unownedInner.checkInner:" + unownedInner.checkInner);
        }
    }

}

class UnownedInner {
    String name = "test";
    int checkInner = 0;
    InnerClass innerClass;

    void method() {
        innerClass = new InnerClass();
    }

    @UnownedOuter
    class InnerClass {
        void myName() {
            checkInner = 0;
            String myname;
            myname = name + name;
            if (myname.equals("testtest")) {
                checkInner++;
            }
        }
    }
}

class UnownedAnnoymous {
    String name = "test";
    static int checkAnnoy;

    void anonymousCapture() {
        Runnable r = new Runnable() {
            @UnownedOuter
            @Override
            public void run() {
                checkAnnoy = 0;
                String myName = name + name;
                if (myName.equals("testtest")) {
                    checkAnnoy++;
                }
            }
        };
        r.run();
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n