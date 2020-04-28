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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest04
 *- @TestCaseName: StmtTest04
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief:在继承的场景下，在同一个函数中出现的字符串常量，只需要调用runtime的MCC_GetOrInsertLiteral函数只会为该字符串常量调用一次。
 *  -#step1: 创建一个有父类的类，在该类中父类中的函数test1()，在该函数中调用一个特定的字符串常量做相关操作;
 *  -#step2: 在main行数用if-else创造两个互斥分支。
 *  -#step3: 校验中间文件SmtmTest04.VtableImpl.s中callassigned &MCC_GetOrInsertLiteral出现的次数：4次。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest04.java
 *- @ExecuteClass: StmtTest04
 *- @ExecuteArgs:
 */

class Father {
    public String name;

    public boolean test1() {
        String str1 = "A" + "A"; // 1
        String str2 = "A" + "A";
        return str1 == str2; // false;
    }
}

public class StmtTest04 extends Father {
    public String name;

    public static void main(String[] args) {
        Father father = new StmtTest04();
        father.name = "ExpectResult"; // 2
        if (father.test1()) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("ExpectResult" + " " + "ExpectResult"); // 4
        }
    }

    // 集成父类
    @Override
    public boolean test1() {
        String str1 = "A" + "A"; // 3
        String str2 = "A" + "A";
        return str1.equals(str2); // true;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n