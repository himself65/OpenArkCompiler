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
 * -@TestCaseID: MethodTest.java
 *- @RequirementName: Java Reflection
 *- @TestCaseName:MethodTest.java
 *- @Title/Destination: for the same method, different Method object is return when call following API twice: Class.getMethods(), Class.getEnclosingMethod(), Class.getDeclaredMethods(), Class.getMethod(String name, Class<?>... parameterTypes), Class.getDeclaredMethod(String name, Class<?>... parameterTypes). tests method with different return return type, parameter type, modifier, annotation and declaring class
 *- @Brief:no: see steps of separate test cases
 *- @Expect:0\n
 *- @Priority: High
 *- @Source: MethodTest.java
 *- @ExecuteClass: MethodTest
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class MethodTest {
    /**
     * Verify that for the same method, different Method object is return when call following API twice:
     * - Class.getMethods(),
     * - Class.getEnclosingMethod(),
     * - Class.getDeclaredMethods(),
     * - Class.getMethod(String name, Class<?>... parameterTypes),
     * - Class.getDeclaredMethod(String name, Class<?>... parameterTypes),
     * if any Method failed the check, will print error message including testcase and method name;
     * after all check is pass, casePassed will be increased;
     */
    private static void classAllMethodTest(Class clazz, String caseName) throws Exception {
        // checkpoint 1. Class.getMethods()
        Method[] methods1 = clazz.getMethods();
        Method[] methods2 = clazz.getMethods();
        checkMethods(methods1, methods2);
        for (Method m : methods1) {
            // checkpoint 2. Class.getMethod(String name)
            Method m1 = clazz.getMethod(m.getName(), m.getParameterTypes());
            Method m2 = clazz.getMethod(m.getName(), m.getParameterTypes());
            if (singleMethodCheck(m1, m2) != 3) {
                System.out.println(caseName + ": " + m1.toGenericString() + " failed");
            }
        }

        // checkpoint 3. Class.getDeclaredMethods()
        methods1 = clazz.getDeclaredMethods();
        methods2 = clazz.getDeclaredMethods();
        checkMethods(methods1, methods2);
        for (Method m : methods1) {
            // checkpoint 4. Class.getDeclaredMethod(String name)
            Method m1 = clazz.getDeclaredMethod(m.getName(), m.getParameterTypes());
            Method m2 = clazz.getDeclaredMethod(m.getName(), m.getParameterTypes());
            if (singleMethodCheck(m1, m2) != 3) {
                System.out.println(caseName + ": " + m1.toGenericString() + " failed for declared method");
            }
        }

        // checkpoint 5. Class.getEnclosingMethod()
        if (clazz.getEnclosingMethod() != null) {
            Method m1 = clazz.getEnclosingMethod();
            Method m2 = clazz.getEnclosingMethod();
            if (singleMethodCheck(m1, m2) != 3) {
                System.out.println(caseName + ": " + m1.toGenericString() + " failed for Enclosing method");
            }
        }

        casesPassed++;
    }

    private static void checkMethods(Method[] methods1, Method[] methods2) {
        for (Method m1: methods1) {
            for (Method m2: methods2) {
                // only when modifers and class and name is same, two Method object is for same method
                if(m1.toString().equals(m2.toString())) {
                    if (singleMethodCheck(m1, m2) != 3) {
                        System.out.println(m1.getName()+" failed in checkMethods");
                    }
                }
            }
        }
    }

    private static int singleMethodCheck(Method m1, Method m2) {
        int passCnt = 0;
        passCnt += m1 == m2 ? 0 : 1;
        passCnt += m1.equals(m2) ? 1 : 0;
        passCnt += m1.hashCode() == m2.hashCode() ? 1 : 0;
        return passCnt;
    }

    static int casesPassed = 0;

    public static void main(String [] args) {
        method01();
        method02();
        method03();
        method04();
        method05();
        method06();
        method07();
        method08();
        method09();
        method10();
        method11();
        System.out.println(casesPassed - 11);
    }

    /**
     *- @TestCaseID: method01
     *- @Title/Destination: for same method in pre-defined class, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. all test in classAllMethodTest() passed for pre-defined class java.lang.Thread
     */
    private static void method01() {
        try {
            classAllMethodTest(Class.forName("java.lang.Thread"), "method01");
        } catch (Exception e) {
            System.out.println("Case method01 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method02
     *- @Title/Destination: for same method in user-defined class with different return_type/parameter_type/modifier/annotation, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. classAllMethodTest() passed for methods with different return type and parameter type
     * -#step2. classAllMethodTest() passed for methods with different access modifier: public, private, protected
     * -#step3. classAllMethodTest() passed for methods with different modifier: transient, final, volatile, static
     * -#step4. classAllMethodTest() passed for methods with annotation
     */
    private static void method02() {
        try {
            classAllMethodTest(Class.forName("TestMethod02"), "method02");
        } catch (Exception e) {
            System.out.println("Case method02 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method03
     *- @Title/Destination: check all test in classAllMethodTest passed for methods in interface
     *- @Brief:no:
     * -step1. check all test in classAllMethodTest passed for methods in interface
     */
    private static void method03() {
        try {
            classAllMethodTest(Class.forName("TestMethod03"), "method03");
        } catch (Exception e) {
            System.out.println("Case method03 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method04
     *- @Title/Destination: for same method in abstract class, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllMethodTest passed for methods in abstract class
     */
    private static void method04() {
        try {
            classAllMethodTest(Class.forName("TestMethod04"), "method04");
        } catch (Exception e) {
            System.out.println("Case method04 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method05
     *- @Title/Destination: for same method in anonymous class, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. get the class instance of anonymous class
     * -#step2. check all test in classAllMethodTest passed for methods in anonymous class
     */
    private static void method05() {
        try {
            // step 1
            Class clazz = Class.forName("MethodTest");
            MethodTest mainClass = new MethodTest();
            Field method = clazz.getDeclaredField("testMethod05");

            classAllMethodTest(method.get(mainClass).getClass(), "method05");
        } catch (Exception e) {
            System.out.println("Case method05 failed with exception" + e);
        }
    }

    private Runnable testMethod05 = new Runnable() {
        private final synchronized int intRunnable() {
            return 1;
        }

        @Override
        public void run() {
            for (int i = 0; i < 10; i++) {
            }
        }
    };

    /**
     *- @TestCaseID: method06
     *- @Title/Destination: for same method in local class(class in a function), certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllMethodTest passed for methods in local class
     */
    private static void method06() {
        class TestMethod06 {
            private final int intLocalMethod() {
                return 9;
            }
            public synchronized String stringLocal() {
                return "a fake behind the fear";
            }
        }
        try {
            classAllMethodTest(Class.forName("MethodTest$1TestMethod06"), "method06");
        } catch (Exception e) {
            System.out.println("Case method06 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method07
     *- @Title/Destination: for same method in inner class, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllMethodTest passed for methods in inner class
     */
    private static void method07() {
        try {
            classAllMethodTest(Class.forName("MethodTest$TestMethod07"), "method07");
        } catch (Exception e) {
            System.out.println("Case method07 failed with exception" + e);
        }
    }

    class TestMethod07 {
        private char charInnerMethod(int a, double... b) {
            return 'l';
        }
        final public double doubleInnerMethod() {
            return 1988.0416;
        }
    }

    /**
     *- @TestCaseID: method08
     *- @Title/Destination: for same method in enum class, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllMethodTest passed for methods in enum class
     */
    private static void method08() {
        try {
            classAllMethodTest(Class.forName("TestMethod08"), "method08");
        } catch (Exception e) {
            System.out.println("Case method08 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method09
     *- @Title/Destination: for same method in parent class, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllMethodTest passed for methods in parent class
     */
    private static void method09() {
        try {
            classAllMethodTest(Class.forName("TestMethod09"), "method09");
        } catch (Exception e) {
            System.out.println("Case method09 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method10
     *- @Title/Destination: for same method in child class with implemented interface, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllMethodTest passed for methods in child class with implemented interface
     */
    private static void method10() {
        try {
            classAllMethodTest(Class.forName("TestMethod10"), "method10");
        } catch (Exception e) {
            System.out.println("Case method10 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: method11
     *- @Title/Destination: for same method in interface implemented by another class, certain APIs return different Method object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllMethodTest passed for methods in interface implemented by another class
     */
    private static void method11() {
        try {
            classAllMethodTest(Class.forName("TestMethod11"), "method11");
        } catch (Exception e) {
            System.out.println("Case method11 failed with exception" + e);
        }
    }
}

class TestMethod02 {
    // step1
    void voidMethod() {}
    boolean booleanMethod() {
        return true;
    }
    byte byteMethod() {
        return (byte)1;
    }
    short shortMethod() {
        return (short)2;
    }
    int intMethod() {
        return 3;
    }
    long longMethod() {
        return 4l;
    }
    char charMethod() {
        return 'a';
    }
    float floatMethod() {
        return 0.9f;
    }
    double doubleMethod() {
        return 0.416;
    }
    String stringMethod() {
        return "The queerest of the queer";
    }
    Runnable[] rArrayMethod() {
        return null;
    }
    boolean[][][][][][][] booleanArray7Method() {
        return new boolean[][][][][][][] {{{{{{{true, false}}}}}}};
    }
    String[][][] stringArray3Method() {
        return new String[][][] {{{"the strangest of the strange"}}};
    }

    int parameterMethod(boolean a, byte b, short c, int d, long e, char f, float g, double h,
                        boolean[] aa, byte[] bb, short[] cc, int[] dd, long[] ee, char[] ff, float[] gg, double[] hh,
                        String i, String[][][] iii, Runnable j, Byte k, Double l, Thread.State m, int... o) {
        return 1;
    }

    // step2
    public int publicIntMethod(boolean a) {
        return 88;
    }
    protected double protectedDoubleMethod(byte a) {
        return 99;
    }
    private String privateStringMethod(String a) {
        return "the coldest of the cool";
    }

    // step3
    strictfp double[][] transientDoubleArrayMethod(float[][] a) {
        return new double[][] {{1988, 4.16}};
    }
    final int finalIntMethod() {
        return 9876;
    }
    synchronized String volatileStringMethod() {
        return "the lamest of the lame";
    }
    static Character staticCharacterMethod(Thread.State a) {
        return 'd';
    }

    //step 4
    @Deprecated
    String[][] stringsAnnotationMethod() {
        return new String[][] {{"the numbest of the dumb"}};
    }
}

interface TestMethod03 {
    String stringInterfaceMethod(int[] a);
    char[][][] charArrayInterfaceMethod(double... a);
}

abstract class TestMethod04 {
    public static final synchronized String[] stringsAbstractArrayMethod() {
        return new String[] {"i hate to see you here", "you choke behind a smile"};
    }
    abstract Runnable runnableAbstractMethod();
}

enum TestMethod08 {
    WHISKY, VODKA, BRANDY, GIN, RUM, TEQUILA;
    private static short shortEnumMethod() {
        return (short)6;
    }
    public final String stringEnumMethod(int... a) {
        return "Dry Martini";
    }
}

class TestMethod09 {
    public int int09Method() {
        return 1;
    }
    protected boolean boolean09Method() {
        return false;
    }
    char[] chars09Method() {
        return new char[] {'a', 'b', 'c'};
    }
    private String string09Method() {
        return "I'm only happy when it rains";
    }
}

class TestMethod10 extends TestMethod09 implements TestMethod11 {
    public boolean boolean09Method() {
        return true;
    }
    public double double10Method() {
        return 0.416;
    }
    public char[] chars09Method() {
        return new char[] {'a'};
    }
    public String string09Method() {
        return "SHALALALA";
    }
}

interface TestMethod11 {
    int int09Method();
    boolean boolean09Method();
    char[] chars09Method();
    String string09Method();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n