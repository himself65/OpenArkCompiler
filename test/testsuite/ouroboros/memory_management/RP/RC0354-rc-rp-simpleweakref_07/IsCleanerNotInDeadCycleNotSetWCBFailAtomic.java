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
 * -@TestCaseID: RefProCase/RefProcessor/src/IsCleanerNotInDeadCycleNotSetWCBFailAtomic.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: RefProcessor basic testcase：在可达的环中构造cleaner对象，并不设置WCB。
 *- @Brief:functionTest
 * -#step1：创建两个带有Cleaner 对象的class，并相互声明了对方作为自己的Field。
 * -#step2：创建第三个类InCycle，将上两个类连成一个可达的环。
 * -#step3：创建InCycle的实例，调用方法一给第一个类对象关于第二个类的Field初始化对象，再将该对象关于第一个类Field赋值为第一个类，
 *          从而形成一个带Cleaner的环。
 * -#step4：方法一调用方法二执行前两个类对象的add方法进行Field的运算，相当于对对象进行了引用，判断运算结果是否正确。
 * -#step5：调用System.gc()进行系统回收，重复步骤4，判断环正确且reference都没释放。
 * -#step6：调用System.gc()进行系统回收，重复步骤3~5。
 * -#step7：调用Runtime.getRuntime().gc()进行系统回收，重复步骤1~6。
 *- @Expect: ExpectResult\n
 *- @Priority: High
 *- @Source:IsCleanerNotInDeadCycleNotSetWCBFailAtomic.java
 *- @ExecuteClass: IsCleanerNotInDeadCycleNotSetWCBFailAtomic
 *- @ExecuteArgs:
 */

import sun.misc.Cleaner;

import java.lang.ref.*;

public class IsCleanerNotInDeadCycleNotSetWCBFailAtomic {
    static int TEST_NUM = 1;
    static int judgeNum = 0;

    public static void main(String[] args) throws Exception {
        judgeNum = 0;
        for (int i = 0; i < TEST_NUM; i++) {
            isCleanerNotInDeadCycleNotSetWCBFailAtomic();
            Runtime.getRuntime().gc();
            isCleanerNotInDeadCycleNotSetWCBFailAtomic();
        }
        if (judgeNum == 0) {
            System.out.println("ExpectResult");
        }
    }

    static void isCleanerNotInDeadCycleNotSetWCBFailAtomic() throws InterruptedException {
        Cycle_BDec_00010_A1_Cleaner cleanerClass1 = new Cycle_BDec_00010_A1_Cleaner();
        InCycle cycleA = new InCycle();
        cycleA.setCleanerCycle(cleanerClass1);
        System.gc();
        boolean result = cycleA.setCleanerCycle(cleanerClass1);
        if (result == false) {
            judgeNum++;
        }
    }
}

class Cycle_BDec_00010_A1_Cleaner {
    static Cleaner cleaner;
    static ReferenceQueue rq = new ReferenceQueue();
    Cycle_BDec_00010_A2_Cleaner cleanerClass2;
    int num;
    int sum;
    static int value;

    Cycle_BDec_00010_A1_Cleaner() {
        cleaner.create(cleanerClass2, null);
        cleanerClass2 = null;
        num = 1;
        sum = 0;
        value = 100;
    }

    void add() {
        sum = num + cleanerClass2.num;
    }
}

class Cycle_BDec_00010_A2_Cleaner {
    Cleaner cleaner;
    static ReferenceQueue rq = new ReferenceQueue();
    Cycle_BDec_00010_A1_Cleaner cleanerClass1;
    int num;
    int sum;
    static int value;

    Cycle_BDec_00010_A2_Cleaner() {
        cleaner.create(cleanerClass1, null);
        cleanerClass1 = null;
        num = 2;
        sum = 0;
        value = 100;
    }

    void add() {
        sum = num + cleanerClass1.num;
    }
}

class InCycle {
    /**
     * 确认环是正确的
     *
     * @param cleaner 传入的是带有Referent的类实例
     * @return 返回布尔值，判断运算的结果正确
     */
    public static boolean ModifyCleanerA1(Cycle_BDec_00010_A1_Cleaner cleaner) {
        cleaner.add();
        cleaner.cleanerClass2.add();
        int nSum = cleaner.sum + cleaner.cleanerClass2.sum;
        if (nSum == 6) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * 设置一个带Cleaner的环
     *
     * @param cleaner 传入的是带有Referent的类实例
     * @return 返回布尔值，判断reference没释放
     */
    public static boolean setCleanerCycle(Cycle_BDec_00010_A1_Cleaner cleaner) {
        cleaner.cleanerClass2 = new Cycle_BDec_00010_A2_Cleaner();
        cleaner.cleanerClass2.cleanerClass1 = cleaner;
        boolean ret;
        ret = ModifyCleanerA1(cleaner);
        System.gc();
        ret = ModifyCleanerA1(cleaner);
        // 环正确，且reference都没释放
        if (ret == true && cleaner.cleaner == null && cleaner.cleanerClass2.cleaner == null && cleaner != null
                && cleaner.cleanerClass2 != null) {
            return true;
        } else {
            return false;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n