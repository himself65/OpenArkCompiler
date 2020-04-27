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
 * -@TestCaseID: JGFSyncBench
 *- @TestCaseName: Thread_JGFSyncBench.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Get the time of thread sync.
 *- @Brief: see below
 * -#step1: 调用JGFInstrumentor.printHeader对header进行设置。
 * -#step2: 声明类JGFSyncBench实现自定义接口JGFSection1。
 * -#step3：创建nThreads+1个Runnable数组和Thread数组,线程里有synchronized同步锁。
 * -#step4: 打点开始时间，启动线程组线程，等待线程结束，打点结束时间,存储运行时间。
 * -#step5：重复步骤1~4 100000次。
 * -#step6：重复步骤1~5。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: JGFSyncBench.java
 *- @ExecuteClass: JGFSyncBench
 *- @ExecuteArgs:
 */

import java.util.Hashtable;

public class JGFSyncBench implements JGFSection1 {
    public static int nThreads;
    private static final int INITSIZE = 1000;
    private static final int MAXSIZE = 100000;
    private static final double TARGETTIME = 10.0;

    public JGFSyncBench(int nThreads) {
        this.nThreads = nThreads;
    }

    public void JGFRun() {
        int i,size;
        double time;
        int counter = 0;
        int shared_cont = 0;

        size=INITSIZE;
        time = 0.0;

        Runnable thoObjects[] = new Runnable[nThreads + 1];
        Thread th[] = new Thread[nThreads + 1];
        CounterClass conTm = new CounterClass(shared_cont);
        CounterClass conTo = new CounterClass(shared_cont);
        JGFInstrumentor.addTimer("Section1:Sync:Method", "synchronisations");

        while (size < MAXSIZE) {
            JGFInstrumentor.resetTimer("Section1:Sync:Method");
            JGFInstrumentor.startTimer("Section1:Sync:Method");

            for(i = 1; i < nThreads + 1; i++) {
                thoObjects[i] = new SyncMethodRunner(i,conTm,size);
                th[i] = new Thread(thoObjects[i]);
                th[i].start();
            }

            thoObjects[0] = new SyncMethodRunner(0, conTm, size);
            thoObjects[0].run();

            for(i = 1; i < nThreads + 1; i++) {
                try {
                    th[i].join();
                } catch (InterruptedException e) {
                    System.out.println("Join is interrupted");
                }
            }

            JGFInstrumentor.stopTimer("Section1:Sync:Method");
            time = JGFInstrumentor.readTimer("Section1:Sync:Method");
            JGFInstrumentor.addOpsToTimer("Section1:Sync:Method", (double) size);
            size *= 2;
        }

        JGFInstrumentor.printPerTimer("Section1:Sync:Method");

        size = INITSIZE;
        time = 0.0;
        JGFInstrumentor.addTimer("Section1:Sync:Object", "synchronisations");

        while (size < MAXSIZE) {
            JGFInstrumentor.resetTimer("Section1:Sync:Object");
            JGFInstrumentor.startTimer("Section1:Sync:Object");

            for(i = 1; i < nThreads + 1; i++) {
                thoObjects[i] = new SyncObjectRunner(i, conTo, size);
                th[i] = new Thread(thoObjects[i]);
                th[i].start();
            }

            thoObjects[0] = new SyncObjectRunner(i, conTo, size);
            thoObjects[0].run();

            for(i = 1; i < nThreads + 1; i++) {
                try {
                    th[i].join();
                } catch (InterruptedException e) {
                    System.out.println("Join is interrupted");
                }
            }

            JGFInstrumentor.stopTimer("Section1:Sync:Object");
            time = JGFInstrumentor.readTimer("Section1:Sync:Object");
            JGFInstrumentor.addOpsToTimer("Section1:Sync:Object",(double) size);
            size *= 2;
        }
        JGFInstrumentor.printPerTimer("Section1:Sync:Object");
    }

    public static void main(String[] argv) {
        nThreads = 4;
        JGFInstrumentor.printHeader(1, 0, nThreads);
        JGFSyncBench sb = new JGFSyncBench(nThreads);
        sb.JGFRun();
        System.out.println(0);
    }
}

class CounterClass {
    int shared_cont;

    public CounterClass(int shared_cont) {
        this.shared_cont = shared_cont;
    }

    public synchronized void update() {
        shared_cont++;
    }
}

class SyncMethodRunner implements Runnable {
    int id;
    int size;
    CounterClass cont;

    public SyncMethodRunner(int id, CounterClass cont ,int size) {
        this.id = id;
        this.cont = cont;
        this.size = size;
    }

    public void run() {
        for(int i = 0; i < size; i++) {
            cont.update();
        }
    }
}

class SyncObjectRunner implements Runnable {
    int id;
    int size;
    CounterClass cont;

    public SyncObjectRunner(int id, CounterClass cont, int size) {
        this.id = id;
        this.cont = cont;
        this.size = size;
    }

    public void run() {
        for(int i = 0; i < size; i++) {
            synchronized(cont) {
                cont.shared_cont++;
            }
        }
    }
}

class JGFInstrumentor{
    private static Hashtable timers;
    private static Hashtable data;

    static {
        timers = new Hashtable();
        data = new Hashtable();
    }

    public static synchronized void addTimer (String name) {
        if (!timers.containsKey(name)) {
            timers.put(name, new JGFTimer(name));
        }
    }

    public static synchronized void addTimer (String name, String opname) {
        if (!timers.containsKey(name)) {
            timers.put(name, new JGFTimer(name,opname));
        }
    }

    public static synchronized void addTimer (String name, String opname, int size) {
        if (!timers.containsKey(name)) {
            timers.put(name, new JGFTimer(name,opname,size));
        }
    }

    public static synchronized void startTimer(String name) {
        if (timers.containsKey(name)) {
            ((JGFTimer) timers.get(name)).start();
        }
    }

    public static synchronized void stopTimer(String name) {
        if (timers.containsKey(name)) {
            ((JGFTimer) timers.get(name)).stop();
        }
    }

    public static synchronized void addOpsToTimer(String name, double count) {
        if (timers.containsKey(name)) {
            ((JGFTimer) timers.get(name)).addOps(count);
        }
    }

    public static synchronized void addTimeToTimer(String name, double added_time) {
        if (timers.containsKey(name)) {
            ((JGFTimer) timers.get(name)).addTime(added_time);
        }
    }

    public static synchronized double readTimer(String name) {
        double time;
        if (timers.containsKey(name)) {
            time = ((JGFTimer) timers.get(name)).time;
        } else {
            time = 0.0;
        }
        return time;
    }

    public static synchronized void resetTimer(String name) {
        if (timers.containsKey(name)) {
            ((JGFTimer) timers.get(name)).reset();
        }
    }

    public static synchronized void printTimer(String name) {
        if (timers.containsKey(name)) {
            ((JGFTimer) timers.get(name)).print();
        }
    }

    public static synchronized void printPerTimer(String name) {
        if (timers.containsKey(name)) {
            ((JGFTimer) timers.get(name)).printPer();
        }
    }

    public static synchronized void storeData(String name, Object obj) {
        data.put(name,obj);
    }

    public static synchronized void retrieveData(String name, Object obj) {
        obj = data.get(name);
    }

    public static synchronized void printHeader(int section, int size,int nThreads) {
        String header;
        String base;
        header = "";
        base = "Java Grande Forum Thread Benchmark Suite - Version 1.0 - Section ";

        switch (section) {
            case 1:
                header = base + "1";
                break;
            case 2:
                switch (size) {
                    case 0:
                        header = base + "2 - Size A";
                        break;
                    case 1:
                        header = base + "2 - Size B";
                        break;
                    case 2:
                        header = base + "2 - Size C";
                        break;
                }
                break;
            case 3:
                switch (size) {
                    case 0:
                        header = base + "3 - Size A";
                        break;
                    case 1:
                        header = base + "3 - Size B";
                        break;
                }
                break;
        }
    }
}

interface JGFSection1 {
    public final int INITSIZE = 10000;
    public final int MAXSIZE = 1000000000;
    public final double TARGETTIME = 1.0;
    public void JGFRun();
}

class JGFTimer {
    public String name;
    public String opName;
    public double time;
    public double opCount;
    public long calls;
    public int size = -1;
    private long start_time;
    private boolean on;

    public JGFTimer(String name, String opName) {
        this.name = name;
        this.opName = opName;
        reset();
    }

    public JGFTimer(String name, String opName, int size) {
        this.name = name;
        this.opName = opName;
        this.size = size;
        reset();
    }

    public JGFTimer(String name) {
        this(name,"");
    }

    public void start() {
        if (on) {
            on = true;
        }
        start_time = System.currentTimeMillis();
    }

    public void stop() {
        time += (double) (System.currentTimeMillis()-start_time) / 1000.;
        if (!on) {
            calls++;
        }
        on = false;
    }

    public void addOps(double count) {
        opCount += count;
    }

    public void addTime(double added_time) {
        time += added_time;
    }

    public void reset() {
        time = 0.0;
        calls = 0;
        opCount = 0;
        on = false;
    }

    public double per() {
        return opCount / time;
    }

    public void print() {
        if (!opName.equals("")) {
            switch(size) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                default:
                    break;
            }
        }
    }

    public void printPer() {
        String name;
        name = this.name;

        // pad name to 40 characters
        while ( name.length() < 40 ) {
            name = name + " ";
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n