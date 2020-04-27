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
 * -@TestCaseID: Maple_MethodHandle_invoke_AnnotationMultiThread
 *- @RequirementName: Java Reflection
 *- @TestCaseName:AnnotationMultiThread.java
 *- @TestCaseName: AnnotationMultiThread
 *- @TestCaseType: Function Testing
 *- @RequirementName: annotation
 *- @Title/Destination: positive test for User-defined Annotation, verify when multiple thread try to use annotations, working fine
 *- @Brief:no:
 * -#step1. User-defined Annotation has field with primitive type, String, ENUM, annotation, class and their array
 * -#step2. verify able to set and get value under multithread
 *- @Expect:0\n
 *- @Priority: Level 1
 *- @Source: AnnotationMultiThread.java AnnoA.java AnnoB.java AnnoC.java AnnoD.java ENUMA.java
 *- @ExecuteClass: AnnotationMultiThread
 *- @ExecuteArgs:
 */

import java.lang.annotation.Annotation;
import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;

public class AnnotationMultiThread {
    static AtomicInteger passCnt = new AtomicInteger();
    public static void main(String[] args) throws ClassNotFoundException {
        ThreadGroup threadGroup = new ThreadGroup("myGroup");

        String[][] expectedGF = new String[][]{
                {"@AnnoA", "enumA=A", "intA=2147483647", "stringA=", "doubleA=4.9E-324", "annoBA=@AnnoB(intB=999)"},
                {"@AnnoB(intB=999)"},
                {"@AnnoC", "annoBA=[@AnnoB(intB=999)]", "intA=[2147483647]", "enumA=[A]", "stringA=[]", "doubleA=[4.9E-324]"}
        };

        String[][] expectedF = new String[][]{
                {"@AnnoA", "enumA=A", "intA=2147483647", "stringA=", "doubleA=4.9E-324", "annoBA=@AnnoB(intB=999)"},
                {"@AnnoB(intB=999)"},
                {"@AnnoC", "annoBA=[@AnnoB(intB=999)]", "intA=[2147483647]", "enumA=[A]", "stringA=[]", "doubleA=[4.9E-324]"},
                {"@AnnoD", "enumA=A", "intA=2147483647", "stringA=", "doubleA=4.9E-324", "annoBA=@AnnoB(intB=999)"},
        };

        new AnnotationGetter("GrandFather", expectedGF, expectedGF, threadGroup, "threadGF").start();
        new AnnotationGetter("Father", expectedF, expectedF, threadGroup, "threadF").start();
        new AnnotationGetter("Interface", expectedF, expectedF, threadGroup, "threadI").start();
        new AnnotationGetter("Son", expectedF, new String[][]{}, threadGroup, "threadS").start();
        new AnnotationGetter("Son2", expectedF, new String[][]{}, threadGroup, "threadS2").start();
        new AnnotationGetter("GrandFather", expectedGF, expectedGF, threadGroup, "threadGF").start();
        new AnnotationGetter("Father", expectedF, expectedF, threadGroup, "threadF").start();
        new AnnotationGetter("Interface", expectedF, expectedF, threadGroup, "threadI").start();
        new AnnotationGetter("Son", expectedF, new String[][]{}, threadGroup, "threadI").start();
        new AnnotationGetter("Son2", expectedF, new String[][]{}, threadGroup, "threadI").start();

        int i = 0;
        while (threadGroup.activeCount() > 0) {
            i++;
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                System.out.println(e);
            }
            if (i > 1000) {
                break;
            }
        }

        System.out.println(passCnt.get() - 200);
    }


    public static boolean checkAllAnnotations(Annotation[] annotations, String[][] expected) {
        String[] actual = new String[annotations.length];

        for (int i = 0; i < annotations.length; i++) {
            actual[i] = annotations[i].toString();
        }
        Arrays.sort(actual);
        if (actual.length != expected.length) {
            return false;
        }
//        System.out.println(Arrays.toString(actual));
        for (int i = 0; i < annotations.length; i++) {
            if (expected[i].length == 1) {
                if (actual[i].compareTo(expected[i][0]) != 0) {
                    System.out.println(actual[i]);
                    System.out.println(expected[i][0]);
                    return false;
                }
            } else {
                for (int j = 0; j < expected[i].length; j++) {
                    if (!actual[i].contains(expected[i][j])) {
                        System.out.println(actual[i]);
                        System.out.println(expected[i][j]);
                        return false;
                    }
                }
            }
        }
        return true;
    }

}

class AnnotationGetter extends Thread {
    String name;
    String[][] expected;
    String[][] expectedD;

    AnnotationGetter(String nameI, String[][] expectedI, String[][] expectedDI, ThreadGroup threadGroup, String thread_name) {
        super(threadGroup, thread_name);
        name = nameI;
        expected = expectedI;
        expectedD = expectedDI;
    }

    @Override
    public void run() {
        try {
            for (int i = 0; i < 10; i++) {
                if (AnnotationMultiThread.checkAllAnnotations(Class.forName(this.name).getAnnotations(), this.expected)) {
                    AnnotationMultiThread.passCnt.incrementAndGet();
                }
                if (AnnotationMultiThread.checkAllAnnotations(Class.forName(this.name).getDeclaredAnnotations(), this.expectedD)) {
                    AnnotationMultiThread.passCnt.incrementAndGet();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
}

@AnnoB
@AnnoA(annoBA = @AnnoB)
@AnnoC()
class GrandFather {
    public void method1(int a, double b, Object... objects) {
    }
}

@SuppressWarnings("all")
@AnnoB
@AnnoA(annoBA = @AnnoB)
@AnnoC()
@AnnoD(annoBA = @AnnoB)
class Father extends GrandFather implements Interface {
    @Override
    public void method1(int a, double b, Object... objects) {
    }

    @Deprecated
    public void method2() {
    }
}

class Son extends Father {
    @Override
    @Deprecated
    public void method2() {
    }
}

class Son2 extends Father {
    @Override
    @Deprecated
    public void method2() {
    }
}

@SuppressWarnings("all")
@AnnoB
@AnnoA(annoBA = @AnnoB)
@AnnoC()
@AnnoD(annoBA = @AnnoB)
interface Interface {

}

// DEPENDENCE: AnnoB.java AnnoC.java AnnoA.java ENUMA.java AnnoD.java
// EXEC:%maple  %f AnnoA.java AnnoB.java AnnoC.java AnnoD.java ENUMA.java %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n