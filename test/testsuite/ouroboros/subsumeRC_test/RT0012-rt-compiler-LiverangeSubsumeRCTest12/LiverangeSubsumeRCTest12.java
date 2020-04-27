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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest12
 *- @TestCaseName: LiverangeSubsumeRCTest12
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:检测在跳转语句（break, continue, return）控制流中,当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 * -#step1:函数1做在continue ； continue tag的跳转for循环里，在if -else里，a1的生命周期都包含住了a3，所以a3会被优化掉
 * -#step2:函数2测试场景：a3 = a2，且在for循环内使用，在if逻辑体，a2的incref和decRef会被优化掉，在else逻辑体内，a2的incRef和decRef不会被优化掉。
 *  校验中间文件LiverangeSubsumeRCTest12.VtableImpl.mpl中有1个IncRef。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest12.java
 *- @ExecuteClass: LiverangeSubsumeRCTest12
 *- @ExecuteArgs:
 */

class A3 {
    public int count = 0;
    public String className = "temp";

    public A3(String name) {
        this.className = name;
    }

    public void changeName(String name) {
        this.className = name;
    }
}

public class LiverangeSubsumeRCTest12 {
    private volatile static int count = 0;
    private A3 defInsideUseOutside = null;

    //def outside, only use inside loop
    public static void onlyUseInsideLoop() {
        A3 a1 = new A3("a1");
        continueTag:
        for (count = 0; count < 10; count++) {
            A3 a3 = a1;
            a1.changeName("a" + count);
            a3.count = count;
            if (count % 8 == 0) {
                a1.changeName("Right"); // a3没有incRef和decRef
                continue continueTag;
            } else {
                a1.className = "a1_" + count; // a3没有incRef和decRef
                continue;
            }
        }
    }

    public static A3 defAndUseInsideLoop() {
        for (count = 0; count < 10; count++) {
            A3 a2 = new A3("a2_i" + count);
            A3 a3 = a2; //因为else分支，这边会有一个incRef
            a2.count = count;
            a3.changeName("null");
            if (count % 4 == 0 && count > 0) {
                a2.changeName("Optimization");
                return a2;
            } else if (count == 3) {
                a3.changeName("NoOptimization");
                return a3;
            }
        }
        return new A3("Error");
    }

    public static void main(String[] args) {
        onlyUseInsideLoop();
        if (defAndUseInsideLoop().className.equals("NoOptimization")) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("ErrorResult");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n