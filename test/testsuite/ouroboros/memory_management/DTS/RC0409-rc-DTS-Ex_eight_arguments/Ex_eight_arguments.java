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
 * -@TestCaseID:Ex_eight_arguments_static.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: more than 8 arguments method test, include static ,no static, has return value, no return, throw
 *                      Exception,not throw Exception
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: Ex_eight_arguments.java
 *- @ExecuteClass: Ex_eight_arguments
 *- @ExecuteArgs:
 */

import java.lang.reflect.Array;

public class Ex_eight_arguments {
    static int check_count = 0;

    public static void main(String[] args) {
        int first = 1988;
        byte second = 2;
        char third = 'g';
        long four = 19881110;
        float five = 601.5f;
        double six = 60.15f;
        String seven = "huawei";
        boolean eight = true;
        Object nine = new String("CC is a cat");
        int[] ten = {2012, 2020, 2019, 2040, 3301};
        Ex_eight_arguments Test = new Ex_eight_arguments();

        //test static method no return no exception
        test01(first, second, third, four, five, six, seven, eight, nine, ten);

        //test no static method no return no exception
        Test.test02(first, second, third, four, five, six, seven, eight, nine, ten);

        //test no static method return value no exception
        first = Test.test03(first, second, third, four, five, six, seven, eight, nine, ten);

        //test no static method no return throws exception
        try {
            Test.test04(first, second, third, four, five, six, seven, eight, nine, ten);
            System.out.println("ErrorResult in test04 not catch Exception");
        } catch (ArrayIndexOutOfBoundsException e) {
            check_count++;
        }

        //test static method return value no exception
        seven = test05(first, second, third, four, five, six, seven, eight, nine, ten);

        //test static method no return value throws exception
        try {
            test06(first, second, third, four, five, six, seven, eight, nine, ten);
            System.out.println("ErrorResult in test06 not catch Exception");
        } catch (IllegalArgumentException e) {
            check_count++;
        }

        //test no static method return value throws exception
        try {
            boolean result7 = Test.test07(first, second, third, four, five, six, seven, eight, nine, ten);
            System.out.println("ErrorResult in test07 not catch Exception");
        } catch (ArithmeticException e) {
            check_count++;
        }

        //test static method return value throws exception
        try {
            int[] result8 = test08(first, second, third, four, five, six, seven, eight, nine, ten);
            System.out.println("ErrorResult in test08 not catch Exception");
        } catch (NegativeArraySizeException e) {
            check_count++;
        }

        //Result judgment
        if (check_count == 12)
            System.out.println("ExpectResult");
        //System.out.println(check_count);
    }

    private static void test01(int first, byte second, char third, long four, float five, double six, String seven,
                               boolean eight, Object nine, int[] ten) {
        /*test static method no return no exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();
        if (first == 1989 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test01");
    }

    private static String test05(int first, byte second, char third, long four, float five, double six, String seven,
                                 Boolean eight, Object nine, int[] ten) {
        /*test static method return value no exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();

        if (first == 1990 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test05");
        return new String("test05 is End");
    }

    private static void test06(int first, byte second, char third, long four, float five, double six, String seven,
                               Boolean eight, Object nine, int[] ten) throws IllegalArgumentException {
        /*test static method no return value throws exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();
        int[][] test_ten = {ten, ten, ten, ten, ten, ten};

        if (first == 1990 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test06");
        Array.getInt(test_ten, 5);
    }

    private static int[] test08(int first, byte second, char third, long four, float five, double six, String seven,
                                Boolean eight, Object nine, int[] ten) throws NegativeArraySizeException {
        /*test static method return value throws exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();
        int[][] test_ten = {ten, ten, ten, ten, ten, ten};

        if (first == 1990 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test08");
        int[] chck = new int[-1];
        return chck;
    }

    private void test02(int first, byte second, char third, long four, float five, double six, String seven,
                        Boolean eight, Object nine, int[] ten) {
        /*test no static method no return no exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();
        if (first == 1989 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test02");
    }

    private int test03(int first, byte second, char third, long four, float five, double six, String seven,
                       Boolean eight, Object nine, int[] ten) {
        /*test no static method return value no exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();
        if (first == 1989 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test03");
        return first;
    }

    private void test04(int first, byte second, char third, long four, float five, double six, String seven,
                        Boolean eight, Object nine, int[] ten) throws ArrayIndexOutOfBoundsException {
        /*test no static method no return throws exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();

        if (first == 1990 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test04");
        ten[5] = 1988;
    }

    private boolean test07(int first, byte second, char third, long four, float five, double six, String seven,
                           Boolean eight, Object nine, int[] ten) throws ArithmeticException {
        /*test no static method return value throws exception*/
        first++;
        second = 56;
        third = (char) second;
        four = (long) third;
        five = (float) four;
        six = (double) five;
        seven = "fei fei is not a pig";
        eight = false;
        nine = (Object) eight;
        ten = ten.clone();
        int[][] test_ten = {ten, ten, ten, ten, ten, ten};

        if (first == 1990 && second == 56 && third == '8' && four == 56 && five == 56f && six == 56f && seven == "fei fei is not a pig"
                && eight == false && nine == (Object) false && ten[4] == 3301)
            check_count++;
        else
            System.out.println("ErrorResult in test07");

        first = first / 0;
        return true;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n