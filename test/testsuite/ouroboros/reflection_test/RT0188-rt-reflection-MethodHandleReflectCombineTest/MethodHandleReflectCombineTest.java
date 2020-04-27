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
 * -@TestCaseID: MethodHandleReflectCombineTest.java
 *- @RequirementName: Java Reflection
 *- @TestCaseName:MethodHandleReflectCombineTest.java
 *- @Title/Destination: Test Field/Method/Constructor object return by MethodHandles.reflectAs(Class<T> expected, MethodHandle target) is heap object
 *- @Brief:no:
 * -#step1. testData including methodHandles for virtual method, static method, private method, constructor with primitive parameter and array parameter, field with primitive type, array type, static field;
 * -#step2. for methodHandles in testData, call reflectAs(Member.class, methodhandle) twice, the Field/Constructor/Method returned is two object
 * -#step3. for private constructor, try to setAccessible(true) for it first and then get method handle for it and check reflectAs return a new Constructor object
 *- @Expect:0\n
 *- @Priority: High
 *- @Source: MethodHandleReflectCombineTest.java
 *- @ExecuteClass: MethodHandleReflectCombineTest
 *- @ExecuteArgs:
 */

import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Member;

import static java.lang.invoke.MethodHandles.lookup;
import static java.lang.invoke.MethodHandles.reflectAs;
import static java.lang.invoke.MethodType.methodType;

public class MethodHandleReflectCombineTest {
    static int passCnt = 0;

    private static <T extends Member> int singleMethodCheck(T m1, T m2) {
        int passCnt = 0;
        passCnt += m1 == m2 ? 0 : 1;
        passCnt += m1.equals(m2) ? 1 : 0;
        passCnt += m1.hashCode() == m2.hashCode() ? 1 : 0;
        return passCnt;
    }

    public static void main(String[] args) {
        try {
            System.out.println(run());
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    private static int run() throws Throwable {
        Object[][] testData = new Object[][]{
            {Member.class, lookup().findVirtual(String.class, "concat", methodType(String.class, String.class))},
            {Member.class, lookup().findStatic(String.class, "copyValueOf", methodType(String.class, char[].class))},
            {Member.class, RATest.LOOKUP().findSpecial(RATest.class, "mhVar", methodType(int.class, Object[].class), RATest.class)},
            {Constructor.class, lookup().findConstructor(RATest.class, methodType(void.class, int.class))},
            {Constructor.class, lookup().findConstructor(RATest.class, methodType(void.class, double[].class))},
            {Field.class, lookup().findStaticGetter(RATest.class, "svi", int.class)},
            {Field.class, lookup().findStaticSetter(RATest.class, "svi", int.class)},
            {Field.class, RATest.LOOKUP().findGetter(RATest.class, "vd", double.class)},
            {Field.class, lookup().findGetter(RATest.class, "vs", String[].class)},
            {Field.class, lookup().findSetter(RATest.class, "vs", String[].class)}
        };

        for (int i = 0; i < testData.length; i++) {
            passCnt += singleMethodCheck(reflectAs((Class) testData[i][0], (MethodHandle) testData[i][1]), reflectAs((Class) testData[i][0], (MethodHandle) testData[i][1])) == 3 ? 1 : 0;
        }

        Constructor privC = Class.forName("RATest").getDeclaredConstructor();
        privC.setAccessible(true);
        MethodHandle raCon = lookup().unreflectConstructor(privC);
        passCnt += singleMethodCheck(privC, reflectAs(Constructor.class, raCon)) == 3 ? 1 : 0;

        return passCnt - 11;
    }
}

class RATest {
    public static MethodHandles.Lookup LOOKUP() {
        return lookup();
    }

    static public double count = 0;
    protected static int svi = 333;
    private double vd = 136;
    String[] vs = {"hi"};

    RATest(int i) {
        count = i;
    }

    public RATest(double[] i) {
        count = i.length;
    }

    private RATest() {
        count = -1;
    }

    private int mhVar(Object... a) {
        return a.length;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n