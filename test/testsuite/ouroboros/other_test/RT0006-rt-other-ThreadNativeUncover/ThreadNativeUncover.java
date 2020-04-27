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
 * -@TestCaseID: Ouroboros/rt_test/ThreadNativeUncover.java
 * - @Title/Destination: ThreadNativeUncover Methods getState()
 * - @Brief:
 * - @Expect:expected.txt
 * - @Priority: High
 * - @Source: ThreadNativeUncover.java
 * - @ExecuteClass: ThreadNativeUncover
 * - @ExecuteArgs:
 */

import java.lang.Thread.State;

public class ThreadNativeUncover {
    private static int res = 99;

    public static void main(String[] args) {
        int result = 2;
        ThreadDemo1();
        if (result == 2 && res == 97) {
            res = 0;
        }
        System.out.println(res);
    }

    public static void ThreadDemo1() {
        Thread thread = new Thread();
        test(thread);
    }

    /**
     * private native int nativeGetStatus(boolean hasBeenStarted);
     * @param thread
     */
    public static void test(Thread thread) {
        try {
            State state = thread.getState();//nativeGetStatus() called by getState();
            if (state.toString().equals("NEW")) {
                ThreadNativeUncover.res = ThreadNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0