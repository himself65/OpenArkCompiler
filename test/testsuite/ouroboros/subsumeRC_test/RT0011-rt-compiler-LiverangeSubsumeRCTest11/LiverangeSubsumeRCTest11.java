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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest11
 *- @TestCaseName: LiverangeSubsumeRCTest11
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:检测在循环语句（if-then, if-then-else, switch）控制流中,当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 * -#step1:函数1测试场景：周期大的对象a1初始化在for循环外，只在for循环内部使用生命周期小的a2；a1的最后调用放到for循环外的if语句里，a2的incRef和decRef会被优化掉。
 * -#step2:函数2测试场景：a2 = a1，q且在for循环内使用，在if逻辑体，a2的incref和decRef不会被优化掉，在else逻辑体内，a2的incRef和decRef会被优化掉。
 * -#step3:函数3测试场景：a2 =a1,在不同的switch--多case里，case1：a1包含了a2,会有优化；case2：没有包含住，不做优化；case3：无代码；case4： a2 = a1;
 *  校验中间文件LiverangeSubsumeRCTest11.VtableImpl.mpl无IncRef。
 *- @Expect:a100\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest11.java
 *- @ExecuteClass: LiverangeSubsumeRCTest11
 *- @ExecuteArgs:
 */

class A2 {
    public int count = 0;
    public String className = "A";

    public A2(String name) {
        this.className = name;
    }

    public void changeName(String name) {
        this.className = name;
    }
}

public class LiverangeSubsumeRCTest11 {
    private volatile static int count = 0;

    public static void onlyUseInsideLoop() {
        A2 a1 = new A2("a1");
        for (count = 0; count < 100; count++) {
            A2 a2 = new A2("a2");
            a2.changeName("a" + count);
            a2.count = count;
        }
        if (count % 10 == 0) {
            a1.changeName("a" + 100);
            System.out.println(a1.className);
        }
    }

    public static void defAndUseInsideLoop() {
        for (count = 0; count < 10; count++) {
            A2 a1 = new A2("a1");
            a1.count = count;
            A2 a2 = a1;
            a2.changeName("null");
            if (count % 2 == 0) {
                a2.changeName("a" + 100);
                a2 = new A2("a10");
                a2.toString();
            } else {
                a1.changeName("a" + 100);
                a1.toString();
            }
        }
    }

    public static void main(String[] args) {
        onlyUseInsideLoop();
        defAndUseInsideLoop();
        new LiverangeSubsumeRCTest11().defInsideAndUseOutsideLoop();
    }

    public void defInsideAndUseOutsideLoop() {
        count = 0;
        do {
            int choice = count % 4;
            A2 a1 = new A2("a2_i" + count);
            A2 a2 = a1;
            switch (choice) {
                case 1:
                    a1.changeName("case 1");
                    break;
                case 2:
                    a2.changeName("case 2");
                    break;
                case 3:
                    break;
                default:
                    a1 = a2;
            }
            count++;
        } while (count < 10);

    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan a100\n