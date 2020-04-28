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
 * -@TestCaseID: FieldTest.java
 *- @RequirementName: Java Reflection
 *- @TestCaseName:FieldTest.java
 *- @Title/Destination: for the same field, different Field object is return when call following API twice: Class.getFields(), Class.getDeclaredFields(), Class.getField(String name), Class.getDeclareedField(String name). tests field with different data type, modifier, annotation and declaring class
 *- @Brief:no: see steps of separate test cases
 *- @Expect:0\n
 *- @Priority: High
 *- @Source: FieldTest.java
 *- @ExecuteClass: FieldTest
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

public class FieldTest {
    /**
     * Verify that for the same field, different Field object is return when call following API twice:
     * - Class.getFields()
     * - Class.getDeclaredFields()
     * - Class.getField(String name)
     * - Class.getDeclareedField(String name)
     * if any Field failed the check, will print error message including testcase and field name;
     * after all check is pass, casePassed will be increased;
     */
    private static void classAllFieldTest(Class clazz, String caseName) throws Exception {
        // checkpoint 1. Class.getFields()
        Field[] fields1 = clazz.getFields();
        Field[] fields2 = clazz.getFields();
        checkFields(fields1, fields2);
        for (Field f : fields1) {
            // checkpoint 2. Class.getDeclaredFields()
            Field f1 = clazz.getField(f.getName());
            Field f2 = clazz.getField(f.getName());
            if (singleFieldCheck(f1, f2) != 3) {
                System.out.println(caseName + ": " + f1.getDeclaringClass().toString() + "." + f1.getName() + " failed");
            }
        }

        // checkpoint 3. Class.getField(String name)
        fields1 = clazz.getDeclaredFields();
        fields2 = clazz.getDeclaredFields();
        checkFields(fields1, fields2);
        for (Field f : fields1) {
            // checkpoint 4. Class.getDeclareedField(String name)
            Field f1 = clazz.getDeclaredField(f.getName());
            Field f2 = clazz.getDeclaredField(f.getName());
            if (singleFieldCheck(f1, f2) != 3) {
                System.out.println(caseName + ": " + f1.getDeclaringClass().toString() + "." + f1.getName() + " failed for declared fields");
            }
        }
        casesPassed++;
    }

    private static void checkFields(Field[] fields1, Field[] fields2) {
        for (Field f1: fields1) {
            for (Field f2: fields2) {
                // only when modifers and class and name is same, two Field object is for same field
                if(f1.toGenericString().equals(f2.toGenericString())) {
                    if (singleFieldCheck(f1, f2) != 3) {
                        System.out.println(f1.getName()+" failed in checkFields");
                    }
                }
            }
        }
    }

    private static int singleFieldCheck(Field f1, Field f2) {
        int passCnt = 0;
        passCnt += f1 == f2 ? 0 : 1;
        passCnt += f1.equals(f2) ? 1 : 0;
        passCnt += f1.hashCode() == f2.hashCode() ? 1 : 0;
        return passCnt;
    }

    static int casesPassed = 0;

    public static void main(String [] args) {
        field01();
        field02();
        field03();
        field04();
        field05();
        field06();
        field07();
        field08();
        field09();
        field10();
        field11();
        field12();
        System.out.println(casesPassed - 12);
    }

    /**
     *- @TestCaseID: field01
     *- @Title/Destination: for same field in pre-defined class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. all test in classAllFieldTest() passed for pre-defined class java.lang.Thread
     */
    private static void field01() {
        try {
            classAllFieldTest(Class.forName("java.lang.Thread"), "field01");
        } catch (Exception e) {
            System.out.println("Case field01 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field02
     *- @Title/Destination: for same field in user-defined class with different type/modifier/annotation, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. classAllFieldTest() passed for fields with different type
     * -#step2. classAllFieldTest() passed for fields with different access modifier: public, private, protected
     * -#step3. classAllFieldTest() passed for fields with different modifier: transient, final, volatile, static
     * -#step4. classAllFieldTest() passed for fields with annotation
     */
    private static void field02() {
        try {
            classAllFieldTest(Class.forName("TestField02"), "field02");
        } catch (Exception e) {
            System.out.println("Case field02 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field03
     *- @Title/Destination: for same field in strictfp class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -step1. all test in classAllFieldTest() passed for class with modifier strictfp
     */
    private static void field03() {
        try {
            classAllFieldTest(Class.forName("TestField03"), "field03");
        } catch (Exception e) {
            System.out.println("Case field03 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field04
     *- @Title/Destination: for same field in interface, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -step1. check all test in classAllFieldTest passed for fields in interface
     */
    private static void field04() {
        try {
            classAllFieldTest(Class.forName("TestField04"), "field04");
        } catch (Exception e) {
            System.out.println("Case field04 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field05
     *- @Title/Destination: for same field in abstract class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllFieldTest passed for fields in abstract class
     */
    private static void field05() {
        try {
            classAllFieldTest(Class.forName("TestField05"), "field05");
        } catch (Exception e) {
            System.out.println("Case field05 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field06
     *- @Title/Destination: for same field in anonymous class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. get the class instance of an anonymous class
     * -#step2. check all test in classAllFieldTest passed for fields in anonymous class
     */
    private static void field06() {
        try {
            // step 1
            Class clazz = Class.forName("FieldTest");
            FieldTest mainClass = new FieldTest();
            Field field = clazz.getDeclaredField("testField06");

            classAllFieldTest(field.get(mainClass).getClass(), "field06");
        } catch (Exception e) {
            System.out.println("Case field06 failed with exception" + e);
        }
    }

    private Runnable testField06 = new Runnable() {
        int intAnonymous = 10;
        public double doubleAnonymous = 1988.416;

        @Override
        public void run() {
            for (int i = 0; i < intAnonymous; i++) {
                intAnonymous += i;
            }
        }
    };

    /**
     *- @TestCaseID: field07
     *- @Title/Destination: for same field in local class(class in a function), certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllFieldTest passed for fields in local class
     */
    private static void field07() {
        class TestField07 {
            private final transient int intLocal = 9;
            public volatile String stringLocal = "a fake behind the fear";
        }
        try {
            classAllFieldTest(Class.forName("FieldTest$1TestField07"), "field07");
        } catch (Exception e) {
            System.out.println("Case field07 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field08
     *- @Title/Destination: for same field in inner class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllFieldTest passed for fields in inner class
     */
    private static void field08() {
        try {
            classAllFieldTest(Class.forName("FieldTest$TestField08"), "field08");
        } catch (Exception e) {
            System.out.println("Case field08 failed with exception" + e);
        }
    }

    class TestField08 {
        volatile private char charInner = 'l';
        public final static transient double doubleInner = 0.88;
    }

    /**
     *- @TestCaseID: field09
     *- @Title/Destination: for same field in user-defined enum class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllFieldTest passed for fields in enum class
     */
    private static void field09() {
        try {
            classAllFieldTest(Class.forName("TestField09"), "field09");
        } catch (Exception e) {
            System.out.println("Case field09 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field10
     *- @Title/Destination: for same field in parent class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllFieldTest passed for fields in parent class
     */
    private static void field10() {
        try {
            classAllFieldTest(Class.forName("TestField10"), "field10");
        } catch (Exception e) {
            System.out.println("Case field10 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field11
     *- @Title/Destination: for same field in child class with implemented interface, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllFieldTest passed for fields in child class with implemented interface
     */
    private static void field11() {
        try {
            classAllFieldTest(Class.forName("TestField11"), "field11");
        } catch (Exception e) {
            System.out.println("Case field11 failed with exception" + e);
        }
    }

    /**
     *- @TestCaseID: field12
     *- @Title/Destination: for same field in interface implemented by another class, certain APIs return different Field object when called twice
     *- @Brief:no:
     * -#step1. check all test in classAllFieldTest passed for fields in interface implemented by another class
     */
    private static void field12() {
        try {
            classAllFieldTest(Class.forName("TestField12"), "field12");
        } catch (Exception e) {
            System.out.println("Case field12 failed with exception" + e);
        }
    }
}

class TestField02 {
    // step1
    boolean booleanField = true;
    byte byteField = (byte)2;
    short shortField = (short)4;
    int intField = 6;
    long longField = 8;
    char charField = 10;
    float floatField = 0.4f;
    double doubleField = 0.8;

    String stringField = "The queerest of the queer";
    Runnable r = null;

    boolean[][][][][][][] booleanArray7 = new boolean[][][][][][][] {{{{{{{true, false}}}}}}};
    String[][][] stringArray3 = new String[][][] {{{"the strangest of the strange"}}};

    // step2
    public int publicInt = 88;
    protected double protectedDouble = 99;
    private String privateString = "the coldest of the cool";

    // step3
    transient double[][] transientDoubleArray = new double[][] {{1988, 4.16}};
    final int finalInt = 9876;
    volatile String volatileString = "the lamest of the lame";
    static Character staticCharacter = 'd';

    //step 4
    @Deprecated
    String[][] stringsAnnotation = new String[][] {{"the numbest of the dumb"}};
}

strictfp class TestField03 {
    float floatField = 4.16f;
    public double doubleField = 19.88;
}

interface TestField04 {
    String stringInterface = "i know what's good for you";
    char[] charInterface = new char[] {'l', 'e', 't'};
}

abstract class TestField05 {
    String[] stringsAbstractArray = {"i hate to see you here", "you choke behind a smile"};
    Runnable[] runnableAbstractArray = {new Runnable() {
            @Override
            public void run() {
            }
        }
    };
}

enum TestField09 {
    WHISKY, VODKA, BRANDY, GIN, RUM, TEQUILA;
    private static short shortEnum = (short)6;
    public final String stringEnum = "Dry Martini";
}

class TestField10 {
    public int int10;
    protected boolean boolean10;
    char[] chars10 = new char[] {'a', 'b', 'c'};
    private String string10 = "I'm only happy when it rains";
}

class TestField11 extends TestField10 implements TestField12 {
    boolean boolean10 = true;
    public double double11 = 0.416;

}

interface TestField12 {
    int int10 = 9;
    boolean boolean10 = false;
    char[] chars10 = new char[] {'a', 'b', 'c'};
    String string10 = "I'm only happy when it rains";
}
// DEPENDENCE: MethodTest.java
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n
