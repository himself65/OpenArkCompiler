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
 * -@TestCaseID: ConstructorTest.java
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ConstructorTest.java
 *- @Title/Destination: for the same constructor, different Constructor object is return when call following API twice: Class.getConstructors(), Class.getEnclosingConstructor(), Class.getDeclaredConstructors(), Class.getConstructor(String name, Class<?>... parameterTypes), Class.getDeclaredConstructor(String name, Class<?>... parameterTypes)
 *- @Brief:no: see steps of separate test cases
 *- @Expect:0\n
 *- @Priority: High
 *- @Source: ConstructorTest.java
 *- @ExecuteClass: ConstructorTest
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;
import java.lang.reflect.Constructor;

public class ConstructorTest {
    /**
     * Verify that for the same constructor, different Constructor object is return when call following API twice:
     * - Class.getConstructors(),
     * - Class.getEnclosingConstructor(),
     * - Class.getDeclaredConstructors(),
     * - Class.getConstructor(String name, Class<?>... parameterTypes),
     * - Class.getDeclaredConstructor(String name, Class<?>... parameterTypes),
     * if any Constructor failed the check, will print error message including testcase and constructor name;
     * after all check is pass, casePassed will be increased;
     */
    private static void classAllConstructorTest(Class clazz, String caseName) throws Exception {
        // checkpoint 1. Class.getConstructors()
        Constructor[] constructors1 = clazz.getConstructors();
        Constructor[] constructors2 = clazz.getConstructors();
        checkConstructors(constructors1, constructors2);
        for (Constructor c : constructors1) {
            // checkpoint 2. Class.getConstructor(String name)
            Constructor c1 = clazz.getConstructor(c.getParameterTypes());
            Constructor c2 = clazz.getConstructor(c.getParameterTypes());
            if (singleConstructorCheck(c1, c2) != 3) {
                System.out.println(caseName + ": " + c1.toGenericString() + " failed");
            }
        }

        // checkpoint 3. Class.getDeclaredConstructors()
        constructors1 = clazz.getDeclaredConstructors();
        constructors2 = clazz.getDeclaredConstructors();
        checkConstructors(constructors1, constructors2);
        for (Constructor c : constructors1) {
            // checkpoint 4. Class.getDeclaredConstructor(String name)
            Constructor c1 = clazz.getDeclaredConstructor(c.getParameterTypes());
            Constructor c2 = clazz.getDeclaredConstructor(c.getParameterTypes());
            if (singleConstructorCheck(c1, c2) != 3) {
                System.out.println(caseName + ": " + c1.toGenericString() + " failed for declared constructor");
            }
        }

        // checkpoint 5. Class.getEnclosingConstructor()
        if (clazz.getEnclosingConstructor() != null) {
            Constructor c1 = clazz.getEnclosingConstructor();
            Constructor c2 = clazz.getEnclosingConstructor();
            if (singleConstructorCheck(c1, c2) != 3) {
                System.out.println(caseName + ": " + c1.toGenericString() + " failed for Enclosing constructor");
            }
        }

        casesPassed++;
    }

    private static void checkConstructors(Constructor[] constructors1, Constructor[] constructors2) {
        for (Constructor c1 : constructors1) {
            for (Constructor c2 : constructors2) {
                // only when modifers and class and name is same, two Constructor object is for same constructor
                if (c1.toString().equals(c2.toString())) {
                    if (singleConstructorCheck(c1, c2) != 3) {
                        System.out.println(c1.getName() + " failed in checkConstructors");
                    }
                }
            }
        }
    }

    private static int singleConstructorCheck(Constructor c1, Constructor c2) {
        int passCnt = 0;
        passCnt += c1 == c2 ? 0 : 1;
        passCnt += c1.equals(c2) ? 1 : 0;
        passCnt += c1.hashCode() == c2.hashCode() ? 1 : 0;
        return passCnt;
    }

    static int casesPassed = 0;

    public static void main(String[] args) {
        constructor01();
        constructor02();
        constructor03();
        constructor04();
        constructor05();
        constructor06();
        constructor07();
        constructor08();
        constructor09();
        constructor10();
        constructor11();
        System.out.println(casesPassed - 11);
    }

    /**
     *- @TestCaseID: constructor01
     *- @Title/Destination: for same constructor in pre-defined class, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. all test in classAllConstructorTest() passed for pre-defined class java.lang.Thread
     */
    private static void constructor01() {
        try {
            classAllConstructorTest(Class.forName("java.lang.Thread"), "constructor01");
        } catch (Exception e) {
            System.out.println("Case constructor01 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor02
     *- @Title/Destination: for same constructor in user-defined class with different parameter_type/modifier/annotation, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. classAllConstructorTest() passed for constructors with different parameter type
     * -#step2. classAllConstructorTest() passed for constructors with different access modifier: public, private, protected
     * -#step3. classAllConstructorTest() passed for constructors with annotation
     */
    private static void constructor02() {
        try {
            classAllConstructorTest(Class.forName("TestConstructor02"), "constructor02");
        } catch (Exception e) {
            System.out.println("Case constructor02 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor03
     *- @Title/Destination: check all test in classAllConstructorTest passed for constructors in class with modifier strictfp
     *- @Brief:no:
     * -step1. check all test in classAllConstructorTest passed for constructors in class with modifier strictfp
     */
    private static void constructor03() {
        try {
            classAllConstructorTest(Class.forName("TestConstructor03"), "constructor03");
        } catch (Exception e) {
            System.out.println("Case constructor03 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor04
     *- @Title/Destination: for same constructor in abstract class, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllConstructorTest passed for constructors in abstract class
     */
    private static void constructor04() {
        try {
            classAllConstructorTest(Class.forName("TestConstructor04"), "constructor04");
        } catch (Exception e) {
            System.out.println("Case constructor04 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor05
     *- @Title/Destination: for same constructor in anonymous class, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. get the class instance of anonymous class by getting constructor value of otter class instance
     * -#step2. check all test in classAllConstructorTest passed for constructors in anonymous class
     */
    private static void constructor05() {
        try {
            Class clazz = Class.forName("ConstructorTest");
            ConstructorTest mainClass = new ConstructorTest();
            Field constructor = clazz.getDeclaredField("testConstructor05");

            classAllConstructorTest(constructor.get(mainClass).getClass(), "constructor05");
        } catch (Exception e) {
            System.out.println("Case constructor05 failed with exception" + e);
        }
    }

    private Runnable testConstructor05 = new Runnable() {
        @Override
        public void run() {
            for (int i = 0; i < 10; i++) {
            }
        }
    };

    /**
     *- @TestCaseID: constructor06
     *- @Title/Destination: for same constructor in local class(class in a function), certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllConstructorTest passed for constructors in local class
     */
    private static void constructor06() {
        class TestConstructor06 {
            private TestConstructor06(int a) {
            }
            public TestConstructor06(String[][][] a) {
            }
        }
        try {
            classAllConstructorTest(Class.forName("ConstructorTest$1TestConstructor06"), "constructor06");
        } catch (Exception e) {
            System.out.println("Case constructor06 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor07
     *- @Title/Destination: for same constructor in inner class, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllConstructorTest passed for constructors in inner class
     */
    private static void constructor07() {
        try {
            classAllConstructorTest(Class.forName("ConstructorTest$TestConstructor07"), "constructor07");
        } catch (Exception e) {
            System.out.println("Case constructor07 failed with exception" + e);
        }
    }

    class TestConstructor07 {
        private TestConstructor07(int a, double... b) {
        }
        public TestConstructor07() {
        }
    }

    /**
     *- @TestCaseID: constructor08
     *- @Title/Destination: for same constructor in user-defined class, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllConstructorTest passed for constructors in
     */
    private static void constructor08() {
        try {
            classAllConstructorTest(Class.forName("TestConstructor08"), "constructor08");
        } catch (Exception e) {
            System.out.println("Case constructor08 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor09
     *- @Title/Destination: for same constructor in father class, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllConstructorTest passed for constructors in father class
     */
    private static void constructor09() {
        try {
            classAllConstructorTest(Class.forName("TestConstructor09"), "constructor09");
        } catch (Exception e) {
            System.out.println("Case constructor09 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor10
     *- @Title/Destination: for same constructor in class extends another class and implements interface, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllConstructorTest passed for constructors in class extends another class and implements interface
     */
    private static void constructor10() {
        try {
            classAllConstructorTest(Class.forName("TestConstructor10"), "constructor10");
        } catch (Exception e) {
            System.out.println("Case constructor10 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: constructor11
     *- @Title/Destination: for same constructor in interface implemented by another class, certain APIs return different Constructor object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllConstructorTest passed for constructors in interface implemented by another class
     */
    private static void constructor11() {
        try {
            Class clazz = (new TestConstructor11()).c.getClass();
            classAllConstructorTest(clazz, "constructor11");
        } catch (Exception e) {
            System.out.println("Case constructor11 failed with exception" + e);
        }
    }
}

class TestConstructor02 {
    // step1
    TestConstructor02() {
    }

    TestConstructor02(boolean a, byte b, short c, int d, long e, char f, float g, double h,
                      boolean[] aa, byte[] bb, short[] cc, int[] dd, long[] ee, char[] ff, float[] gg, double[] hh,
                      String i, String[][][] iii, Runnable j, Byte k, Double l, Thread.State m, int... o) {
    }

    // step2
    public TestConstructor02(int a) {}
    TestConstructor02(byte a) {}
    protected TestConstructor02(String a) {}
    private TestConstructor02(Runnable[] a) {}

    // step3
    @Deprecated
    TestConstructor02(int... a) {}
}

strictfp class TestConstructor03 {
    public TestConstructor03(float... a) {
    }
    TestConstructor03(float a, double b) {
    }
}

abstract class TestConstructor04 {
    public TestConstructor04() {
    }

    TestConstructor04(int a) {
    }
}

enum TestConstructor08 {
    WHISKY, VODKA, BRANDY, GIN, RUM, TEQUILA;
    TestConstructor08() {
    }

    TestConstructor08(String a) {
    }
}

class TestConstructor09 {
    public TestConstructor09() {
    }
    TestConstructor09(int... a) {
    }
    protected TestConstructor09(double[] a) {
    }
    private TestConstructor09(int a) {
    }
}

class TestConstructor10 extends TestConstructor09 {
    public TestConstructor10() {
    }
    TestConstructor10(int... a) {
    }
    protected TestConstructor10(double[] a) {
    }
    private TestConstructor10(int a) {
    }
}

class TestConstructor11 {
    public Object c;
    public TestConstructor11( ) {
        class ClassA {
        }
        c = new ClassA();
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n