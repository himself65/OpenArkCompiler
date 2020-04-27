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
 * -@TestCaseID: RefProCase/RefProcessor/src/NotIsCleanerNotInDeadCycleNotSetWCBFailAtomic.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: RefProcessor basic testcase: 在可达的环中，构造对象，不设置WCB
 *- @Brief:functionTest
 * -#step1：创建两个带有弱引用对象的class，弱引用申请后通过get方法判断非空，并相互声明对方类作为自己的Field。
 * -#step2：创建第三个类InCycle，将上两个类连成一个可达的环。
 * -#step3：创建InCycle的实例，调用方法一给第一个类对象关于第二个类的Field初始化对象，再将该对象关于第一个类Field赋值为第一个类，
 *          从而形成一个环。
 * -#step4：方法一调用方法二执行前两个类对象的add方法进行Field的运算，相当于对对象进行了引用，判断运算结果是否正确。
 * -#step5：调用System.gc()进行系统回收，重复步骤3~4。
 * -#step6：调用Runtime.getRuntime().gc()进行系统回收，重复步骤1~5。
 *- @Expect: ExpectResult\n
 *- @Priority: High
 *- @Source:NotIsCleanerNotInDeadCycleNotSetWCBFailAtomic.java
 *- @ExecuteClass: NotIsCleanerNotInDeadCycleNotSetWCBFailAtomic
 *- @ExecuteArgs:
 */

import java.lang.ref.*;

public class NotIsCleanerNotInDeadCycleNotSetWCBFailAtomic {
    static int TEST_NUM = 1;
    static int judgeNum = 0;

    public static void main(String[] args) throws Exception {
        judgeNum = 0;
        for (int i = 0; i < TEST_NUM; i++) {
            notIsCleanerNotInDeadCycleNotSetWCBFailAtomic();
            Runtime.getRuntime().gc();
            notIsCleanerNotInDeadCycleNotSetWCBFailAtomic();
        }
        if (judgeNum == 0) {
            System.out.println("ExpectResult");
        }
    }

    static void notIsCleanerNotInDeadCycleNotSetWCBFailAtomic() throws InterruptedException {
        Cycle_BDec_00010_A1 cycleMember = new Cycle_BDec_00010_A1();
        InCycle cycleA = new InCycle();
        cycleA.setCycle(cycleMember);
        System.gc();
        boolean result = cycleA.setCycle(cycleMember);
        if (result == false) {
            judgeNum++;
        }
    }
}

class Cycle_BDec_00010_A1 {
    Reference memberRef1;
    static ReferenceQueue rq = new ReferenceQueue();
    Cycle_BDec_00010_A2 partner2;
    int num;
    int sum;
    static int value;
    static StringBuffer obj = new StringBuffer("weak");

    Cycle_BDec_00010_A1() {
        obj = new StringBuffer("weak");
        memberRef1 = new WeakReference<Object>(obj, rq);
        if (memberRef1.get() == null) {
            assert false;
        }
        obj = null;
        partner2 = null;
        num = 1;
        sum = 0;
        value = 100;
    }

    void add() {
        sum = num + partner2.num;
    }
}

class Cycle_BDec_00010_A2 {
    Reference memberRef2;
    static ReferenceQueue rq = new ReferenceQueue();
    Cycle_BDec_00010_A1 partner1;
    int num;
    int sum;
    static int value;
    static StringBuffer obj = new StringBuffer("weak");

    Cycle_BDec_00010_A2() {
        obj = new StringBuffer("weak");
        memberRef2 = new WeakReference<Object>(obj, rq);
        if (memberRef2.get() == null) {
            assert false;
        }
        obj = null;
        partner1 = null;
        num = 2;
        sum = 0;
        value = 100;
    }

    void add() {
        sum = num + partner1.num;
    }
}

class InCycle {
    /**
     * 确认环是正确的
     *
     * @param refInstance 传入的是带有Referent的类实例
     * @return true:正确；false：错误
     */
    public static boolean ModifyA1(Cycle_BDec_00010_A1 refInstance) {
        refInstance.add();
        refInstance.partner2.add();
        int nSum = refInstance.sum + refInstance.partner2.sum;
        if (nSum == 6) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * 设置一个带Referent的环
     *
     * @param refInstance 传入的是带有Referent的类实例
     * @return true:正确；false：错误
     */
    public static boolean setCycle(Cycle_BDec_00010_A1 refInstance) {
        refInstance.partner2 = new Cycle_BDec_00010_A2();
        refInstance.partner2.partner1 = refInstance;
        boolean ret;
        ret = ModifyA1(refInstance);
        if (ret == true) {
            return true;
        } else {
            System.out.println(Thread.currentThread().getStackTrace()[2].getClassName() + " ret != true");
            return false;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n