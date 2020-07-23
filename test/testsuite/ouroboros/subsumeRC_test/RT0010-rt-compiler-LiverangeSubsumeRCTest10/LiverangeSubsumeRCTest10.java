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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest10
 *- @TestCaseName: LiverangeSubsumeRCTest10
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:检测在循环语句（for, while, do-while）控制流中,当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 * -#step1:函数1测试场景：周期大的对象a1初始化在for循环外，只在for循环内部使用生命周期小的a2；a2的incref和decRef都应该被优化掉
 * -#step2:函数2测试场景：周期大的对象a1和生命周期小的对象a2对象初始化和使用都在for循环内，因为a1的生命周期完全覆盖了a2的生命周期，a2的incref和decRef都应该被优化掉；
 * -#step3:函数3测试场景：对象初始化在do-while循环内，循环之后，仍有使用；a2的生命周期因为被完全覆盖，所以它的incref和decRef都应该被优化掉；
 *  校验中间文件LiverangeSubsumeRCTest10.VtableImpl.mpl中无IncRef。
 *- @Expect:a1_1a1_1a1_1a1_1a1_1a1_1a1_1a1_1a1_1a1_1100ExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest10.java
 *- @ExecuteClass: LiverangeSubsumeRCTest10
 *- @ExecuteArgs:
 */
class A {
    public int count = 0;
    public String className = "A";

    public A(String name) {
        this.className = name;
    }

    public void changeName(String name) {
        this.className = name;
    }
}

public class LiverangeSubsumeRCTest10 {
    private static volatile int count = 0;
    private static A infiniteLoop = null;
    private A defInsideUseOutside = null;

    public static boolean onlyUseInsideLoop() {
        A a1 = new A("a1");
        for (count = 0; count < 100; count++) {
            A a2 = a1;
            a2.changeName("a" + count);
            a2.count = count;
            if (count == 99)
                a2.toString();
        }
        return a1.className.equals("a99");
    }

    public static void defAndUseInsideLoop() {
        for (count = 0; count < 10; count++) {
            A a1 = new A("a1_" + count);
            a1.changeName("a1");
            A a2 = a1;
            for (int j = 0; j < 2; j++) {
                a2.changeName("a1_" + j);
            }
            System.out.print(a1.className);
        }
    }

    public static void main(String[] args) {
        defAndUseInsideLoop();
        new LiverangeSubsumeRCTest10().defInsideAndUseOutsideLoop();
        if (onlyUseInsideLoop()) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("ErrorResult");
        }
    }

    public void defInsideAndUseOutsideLoop() {
        count = 0;
        do {
            this.defInsideUseOutside = new A("a1_i" + count);
            A a2 = this.defInsideUseOutside;
            a2.count = count;
            for (int j = 0; j < 2; j++)
                a2 = new A("a2_i" + count + "_j" + j);
            if (count == 99)
                a2.toString();
            count++;
        } while (this.defInsideUseOutside.count < 100 && count < 100);
        System.out.print(count);
    }


}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full a1_1a1_1a1_1a1_1a1_1a1_1a1_1a1_1a1_1a1_1100ExpectResult\n