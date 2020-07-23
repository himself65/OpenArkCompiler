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
// ASSERT: scan-full 0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n81\n82\n83\n84\n85\n86\n87\n88\n89\n90\n91\n92\n93\n94\n95\n96\n97\n98\n99\n