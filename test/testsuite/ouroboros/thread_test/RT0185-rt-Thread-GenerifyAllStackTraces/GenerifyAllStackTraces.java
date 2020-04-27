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
 * -@TestCaseID: GenerifyAllStackTraces
 *- @TestCaseName: GenerifyAllStackTraces.java
 *- @RequirementName: Unwind
 *- @Title/Destination: Generified basic unit test of Thread.getAllStackTraces()
 *- @Brief: see below
 * -#step1： 创造两个线程组，每个线程都有相同同步锁，解锁后等待10ms，
 * -#step2： 定义ThreadOne和ThreadTwo类，该类均继承Thread类，分别定义run函数，会调用wait()方法依次调用A,B,C方法，最后获取当前线程的栈信
 *           息，并检查。
 * -#step3： 定义DumpThread类，该类继承Thread类，定义run函数，等待ThreadOne和ThreadTwo阻塞获取栈信息，通知ThreadOne继续。
 * -#step4： 等待线程组中的ThreadOne和ThreadTwo对象运行结束。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: GenerifyAllStackTraces.java
 *- @ExecuteClass: GenerifyAllStackTraces
 *- @ExecuteArgs:
 */


import java.util.Map;

public class GenerifyAllStackTraces {
    private static Object go = new Object();
    private static String[] methodNames = {"run", "A", "B", "C", "Done"};
    private static int DONE_DEPTH = 5;
    private static boolean testFailed = false;
    private static Thread one;
    private static Thread two;
    private static boolean trace = false;

    public static void main(String[] args) throws Exception {
        if (args.length > 0 && args[0].equals("trace")) {
            trace = true;
        }

        one = new ThreadOne();
        two = new ThreadTwo();
        one.start();
        two.start();

        DumpThread dt = new DumpThread();
        dt.start();

        try {
            one.join();
            two.join();
        } finally {
            dt.shutdown();
        }

        if (testFailed) {
            throw new RuntimeException("Test Failed.");
        }
        System.out.println(0);
    }

    static class DumpThread extends Thread {
        private volatile boolean finished = false;

        @Override
        public void run() {
            int depth = 2;
            while (!finished) {
                // At each iterator, wait until ThreadOne blocks to wait for thread dump.
                // Then dump stack trace and notify ThreadOne to continue.
                try {
                    sleep(2000);
                    dumpStacks(depth);
                    depth++;
                    finishDump();
                } catch (Exception e) {
                    e.printStackTrace();
                    testFailed = true;
                }
            }
        }

        public void shutdown() throws InterruptedException {
            finished = true;
            join();
        }
    }

    static class ThreadOne extends Thread {
        @Override
        public void run() {
            System.out.println();
            A();
        }

        private void A() {
            waitForDump();
            B();
        }

        private void B() {
            waitForDump();
            C();
        }

        private void C() {
            waitForDump();
            Done();
        }

        private void Done() {
            waitForDump();

            // Get stack trace of current thread

            StackTraceElement[] stack = getStackTrace();
            try {
                checkStack(this, stack, DONE_DEPTH);
            } catch (Exception e) {
                e.printStackTrace();
                testFailed = true;
            }
        }
    }

    static class ThreadTwo extends Thread {
        @Override
        public void run() {
            System.out.println();
            A();
        }

        private void A() {
            waitForDump();
            B();
        }

        private void B() {
            waitForDump();
            C();
        }

        private void C() {
            waitForDump();
            Done();
        }

        private void Done() {
            waitForDump();

            // Get stack trace of current thread
            StackTraceElement[] stack = getStackTrace();
            try {
                checkStack(this, stack, DONE_DEPTH);
            } catch (Exception e) {
                e.printStackTrace();
                testFailed = true;
            }
        }
    }

    static private void waitForDump() {
        synchronized (go) {
            try {
                go.wait();
            } catch (Exception e) {
                throw new RuntimeException("Unexpected exception" + e);
            }
        }
    }

    static private void finishDump() {
        synchronized (go) {
            try {
                go.notifyAll();
            } catch (Exception e) {
                throw new RuntimeException("Unexpected exception" + e);
            }
        }
    }

    public static void dumpStacks(int depth) throws Exception {
        // Get stack trace of another thread
        StackTraceElement[] stack = one.getStackTrace();
        StackTraceElement[] stacks = two.getStackTrace();
        checkStack(one, stack, depth);
        checkStack(two, stacks, depth);

        // Get stack traces of all Threads
        for (Map.Entry<Thread, StackTraceElement[]> entry : Thread.getAllStackTraces().entrySet()) {
            Thread t = entry.getKey();
            stack = entry.getValue();
            if (t == null || stack == null) {
                throw new RuntimeException("Null thread or stacktrace returned");
            }
            if (t == one) {
                checkStack(t, stack, depth);
            }
            if (t == two) {
                checkStack(t, stacks, depth);
            }
        }
    }

    private static void checkStack(Thread t, StackTraceElement[] stack, int depth) throws Exception {
        if (trace) {
            printStack(t, stack);
        }
        int frame = stack.length - 1;
        for (int i = 0; i < depth && frame >= 0; i++) {
            if (!stack[frame].getMethodName().equals(methodNames[i])) {
                throw new RuntimeException("Expected " + methodNames[i] + " in frame " + frame + " but got " +
                        stack[frame].getMethodName());
            }
            frame--;
        }
    }

    private static void printStack(Thread t, StackTraceElement[] stack) {
        System.out.println(t + " stack: (length = " + stack.length + ")");
        if (t != null) {
            for (int j = 0; j < stack.length; j++) {
                System.out.println(stack[j]);
            }
            System.out.println();
        }
    }
}



// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n