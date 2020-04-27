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
 * -@TestCaseID: Basic
 *- @TestCaseName: Thread_Basic.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Basic functional test of ThreadLocal
 *- @Brief: see below
 * -#step1: Define a test class.
 * -#step2: Create thread instance.
 * -#step3: Start the thread.
 * -#step4: Wait for the threads to finish.
 * -#step5: Check that result without exception.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: Basic.java
 *- @ExecuteClass: Basic
 *- @ExecuteArgs:
 */

public class Basic {
    static ThreadLocal n = new ThreadLocal() {
        int i = 0;

        protected synchronized Object initialValue() {
            return new Integer(i++);
        }
    };

    public static void main(String[] args) throws Exception {
        int threadCount = 100;
        Thread[] th = new Thread[threadCount];
        final int[] x = new int[threadCount];

        // Start the threads
        for (int i = 0; i < threadCount; i++) {
            th[i] = new Thread() {
                public void run() {
                    int threadId = ((Integer) (n.get())).intValue();
                    for (int j = 0; j < threadId; j++) {
                        x[threadId]++;
                        yield();
                    }
                }
            };
            th[i].start();
        }

        // Wait for the threads to finish
        for (int i = 0; i < threadCount; i++)
            th[i].join();

        // Check results
        for (int i = 0; i < threadCount; i++) {
            if (x[i] != i)
                throw (new Exception("x[" + i + "] =" + x[i]));
            System.out.println(x[i]);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\s*1\s*2\s*3\s*4\s*5\s*6\s*7\s*8\s*9\s*10\s*11\s*12\s*13\s*14\s*15\s*16\s*17\s*18\s*19\s*20\s*21\s*22\s*23\s*24\s*25\s*26\s*27\s*28\s*29\s*30\s*31\s*32\s*33\s*34\s*35\s*36\s*37\s*38\s*39\s*40\s*41\s*42\s*43\s*44\s*45\s*46\s*47\s*48\s*49\s*50\s*51\s*52\s*53\s*54\s*55\s*56\s*57\s*58\s*59\s*60\s*61\s*62\s*63\s*64\s*65\s*66\s*67\s*68\s*69\s*70\s*71\s*72\s*73\s*74\s*75\s*76\s*77\s*78\s*79\s*80\s*81\s*82\s*83\s*84\s*85\s*86\s*87\s*88\s*89\s*90\s*91\s*92\s*93\s*94\s*95\s*96\s*97\s*98\s*99