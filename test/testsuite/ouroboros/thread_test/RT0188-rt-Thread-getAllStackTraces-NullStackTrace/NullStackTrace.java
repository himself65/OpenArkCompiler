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
 * -@TestCaseID: getAllStackTrace_NullStackTrace
 *- @TestCaseName: NullStackTrace.java
 *- @RequirementName: getAllStackTrace
 *- @Title/Destination: Thread.getAllStackTrace() on runnable threads, should not return null
 *- @Brief: see below
 * -#step1: 创建一个线程实例
 * -#step2: 启动线程
 * -#step3: 用Thread.getAllStackTrace() 打印线程，不能返回为空
 * -#step4: 重复1-3步骤1000次.
 *- @Expect: Passed
 *- @Priority: High
 *- @Source: NullStackTrace.java
 *- @ExecuteClass: NullStackTrace
 *- @ExecuteArgs:
 */
import java.util.Map;

public class NullStackTrace extends Thread{
    static final int TIMES = 1000;

    public static void main(String[] args) {
        for (int i = 0; i < TIMES; i++) {
            Thread t = new Thread();
            t.start();

            Map m = getAllStackTraces();
            String stFinish = m.keySet().toString();
            if (stFinish == null)
                throw new RuntimeException("Failed: Thread.getAllStackTrace should not return null");
        }
        System.out.println("Passed");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan Passed