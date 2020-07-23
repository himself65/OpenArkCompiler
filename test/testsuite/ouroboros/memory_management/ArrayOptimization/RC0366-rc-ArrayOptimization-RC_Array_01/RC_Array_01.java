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
 * -@TestCaseID:maple/runtime/rc/optimization/RC_Array_01.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Scenario testing for RC optimization: testing various scenes of 1D, 2D, 3D int array objects，including：
 *                   1.Parameter modification / parameter has not been modified
 *                   2.final、static
 *                   3.As a constructor fun
 *                   4.Function call
 *                   5.Object Passing
 *                   6.return constant; variable; function call
 *                   7.Inherited as a parent class; child class inherits the parent class; interface call;
 *                   8.Exception
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_Array_01.java
 *- @ExecuteClass: RC_Array_01
 *- @ExecuteArgs:
 */

import java.lang.reflect.Array;
import java.util.Arrays;

class Base_001 {
    //Parent interface
    volatile int[] base1;
    int[][] base2;
    int[][] base21;
    int[][][] base3;
}

public class RC_Array_01 extends Base_001 {
    static int check_count = 0;
    static int[] arr1 = {10, 20, 30, 40};
    static int[][] arr2 = {{10, 20, 30, 40}, {40, 50}, {60}};
    static int[][] arr21 = {{40, 50, 60, 30}, {70, 80}, {90}};
    static int[][][] arr3 = {arr2, arr21};

    private RC_Array_01() {
        base1 = new int[]{10, 20, 30, 40};
        base2 = new int[][]{{10, 20, 30, 40}, {40, 50}, {60}};
        base21 = new int[][]{{40, 50, 60, 30}, {70, 80}, {90}};
        base3 = new int[][][]{{{10, 20, 30, 40}, {40, 50}, {60}}, {{40, 50, 60, 30}, {70, 80}, {90}}};
    }

    private RC_Array_01(int[] intar) {
        base1 = intar;
    }

    private RC_Array_01(int[][] intarr) {
        base2 = intarr;
    }

    private RC_Array_01(int[][][] intarrr) {
        base3 = intarrr;
    }

    private RC_Array_01(int[] intar, int[][] intarr, int[][][] intarrr) {
        base1 = intar;
        base2 = intarr;
        base3 = intarrr;
    }

    public static void main(String[] args) {
        final int[] TEST1 = {10, 20, 30, 40};
        final int[][] TEST2 = {TEST1, {40, 50}, {60}};
        final int[][] TEST21 = {{40, 50, 60, 30}, {70, 80}, {90}};
        final int[][][] TEST3 = {TEST2, TEST21};
        //Initialization check
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step1");

        //test01 interface call, internal initialization array, do not modify the parameter value, only judge
        test01(4, TEST1, TEST2, TEST3);
        test01(4, arr1, arr2, arr3);
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step2");

        //test02 interface call, call function change to modify the parameter value and judge
        test02(4, TEST1, TEST2, TEST3);
        test02(4, arr1, arr2, arr3);
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2 && arr1.length == 4 && arr2.length == 3 && arr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step3");

        //The test03 interface call, call the RC_Array_01_test01 function return value to the parameter third and judged.
        //RC_Array_01_test01,return 2D array，Call the multi-parameter constructor of RC_Array_01, and assign a value to
        //the newly created object field, and judge the result
        test03(TEST2);
        test03(arr2);
        if (TEST2.length == 3 && arr2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in step4");

        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_01 () no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        //Test points: inheritance, constructor, return function call, constant, variable, do not receive return value
        RC_Array_01_test02();

        //Get an array object by returning a function call
        int[] getarr1 = RC_Array_get01();
        int[][] getarr2 = RC_Array_get02();
        int[][][] getarr3 = RC_Array_get03();
        if (getarr1.length == 4 && getarr2.length == 3 && getarr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step5");

        //Exception testing
        Boolean ret = RC_Array_Exception();
        if (ret == true)
            check_count++;
        else
            System.out.println("RC_Array_Exception ErrorResult");

        //Result judgment
        //System.out.println(check_count);
        if (check_count == 24)
            System.out.println("ExpectResult");

    }

    private static void test01(int first, int[] second, int[][] third, int[][][] four) {
        //test01 interface call, internal initialization array, do not modify the parameter value, only judge
        int[] xyz = {23, 24, 25, 26};
        int[][] xyz2 = {{23, 24, 25, 26}, {23, 24}, {23}};
        int[][][] xyz3 = {xyz2, xyz2};
        if (second.length == 4 && third.length == 3 && four.length == 2
                && xyz.length == 4 && xyz2.length == 3 && xyz3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in test01");
    }

    private static Object change(Object temp1, Object temp2) {
        temp1 = temp2;
        return temp1;
    }

    private static void test02(int first, int[] second, int[][] third, int[][][] four) {
        //test02 interface call, call function change to modify the parameter value and judge
        int[] xyz = {23, 24, 25, 26};
        int[][] xyz2 = {{23, 24, 25, 26}, {23, 24}, {23}};
        int[][][] xyz3 = {xyz2, xyz2};
        second = (int[]) change(second, xyz);
        third = (int[][]) change(third, xyz2);
        four = (int[][][]) change(four, xyz3);
        if (second.length == 4 && third.length == 3 && four.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in test02");
    }

    private static void test03(int[][] third) {
        //The test03 interface is called, and the RC_Array_01_test01 function call is assigned as the return value to
        // the parameter third and judged.
        third = RC_Array_01_test01();
        if (third.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in test03");
    }

    private static int[] RC_Array_get01() {
        //Call the 1D array returned by RC_Array_set01
        return RC_Array_set01();
    }

    private static int[][] RC_Array_get02() {
        //Call the 2D array returned by RC_Array_set02
        return RC_Array_set02();
    }

    private static int[][][] RC_Array_get03() {
        //Call the 3D array returned by RC_Array_set03
        return RC_Array_set03();
    }

    private static int[] RC_Array_set01() {
        //return 1D array，Call the constructor of the 1D array parameter of RC_Array_set01, and assign a value to the
        // field of the newly created object, and judge the result
        int[] value1 = {23, 24, 25, 26};
        RC_Array_01 rctest = new RC_Array_01(value1);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new int[]{88, 10, 02, 11};
        if (rctest.base1.length == 4)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set01");
        return rctest.base1;
    }

    private static int[][] RC_Array_set02() {
        //return 2D array，Call the constructor of the 2D array parameter of RC_Array_set02, and assign a
        // value to the domain of the newly created object, and judge the result
        int[][] value2 = {{23, 24, 25, 26}, {23, 24}, {23}};
        RC_Array_01 rctest = new RC_Array_01(value2);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base2 = new int[][]{{88, 10, 02, 11}, {10, 92}, {16}};
        if (rctest.base2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set02");
        return rctest.base2;
    }

    private static int[][][] RC_Array_set03() {
        //return 3D array，Call the constructor of the 3D array parameter of RC_Array_set03, and assign a value to the
        // field of the newly created object, and judge the result
        int[][][] value3 = {{{10, 20, 30, 40}, {40, 50}, {60}}, {{40, 50, 60, 30}, {70, 80}, {90}}};
        RC_Array_01 rctest = new RC_Array_01(value3);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base3 = new int[][][]{{{88, 10, 02, 11}, {10, 92}, {12}}, {{88, 10, 02, 11}, {10, 92}, {16}}};
        if (rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set03");
        return rctest.base3;
    }

    private static int[][] RC_Array_01_test01() {
        //return 2D array，Call the multi-parameter constructor of RC_Array_01, and assign a value to the newly created
        // object field, and judge the result
        int[] value1 = {23, 24, 25, 26};
        int[][] value2 = {{23, 24, 25, 26}, {23, 24}, {23}};
        int[][][] value3 = {value2, value2};
        RC_Array_01 rctest = new RC_Array_01(value1, value2, value3);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new int[]{88, 10, 02, 11};
        rctest.base2 = new int[][]{{88, 10, 02, 11}, {10, 92}, {16}};
        rctest.base21 = new int[][]{{88, 10, 02, 11}, {10, 92}, {12}};
        rctest.base3 = new int[][][]{{{88, 10, 02, 11}, {10, 92}, {12}}, {{88, 10, 02, 11}, {10, 92}, {16}}};
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_01_test01");
        return rctest.base21;
    }

    private static int[] RC_Array_final01() {
        final int[] VALUE1 = {23, 24, 25, 26};
        return VALUE1;
    }

    private static int[][] RC_Array_final02() {
        final int[][] VALUE2 = {{23, 24, 25, 26}, {23, 24}, {23}};
        return VALUE2;
    }

    private static int[][][] RC_Array_final03() {
        final int[][][] VALUE3 = {{{10, 20, 30, 40}, {40, 50}, {60}}, {{40, 50, 60, 30}, {70, 80}, {90}}};
        return VALUE3;
    }

    private static int[][] RC_Array_01_test02() {
        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_01 () no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        final int[] VALUE1 = RC_Array_final01();
        final int[][] VALUE2 = RC_Array_final02();
        final int[][][] VALUE3 = RC_Array_final03();
        RC_Array_01 rctest = new RC_Array_01();
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_01_test02");
        rctest.base1 = VALUE1;
        rctest.base2 = VALUE2;
        rctest.base21 = VALUE2;
        rctest.base3 = VALUE3;
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_01_test02_2");
        return VALUE2;
    }

    private static Boolean RC_Array_Exception() {
        //Exception test，exclude NullPointerException，ArrayIndexOutOfBoundsException and so on
        int check = 0;
        int[] value1 = RC_Array_final01();
        int[][] value2 = RC_Array_final02();
        int[][][] value3 = RC_Array_final03();
        //Is the value as expect after the assignment?
        if (value1.length == 4 && value2.length == 3 && value3.length == 2)
            check++;
        else
            System.out.println("ErrorResult in RC_Array_Exception——1");
        //ArrayIndexOutOfBoundsException
        try {
            Array.getInt(value1, 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            value1[5] = 23;
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.setInt(value1, 5, 10);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getInt(RC_Array_final01(), 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getInt(value2[5], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getInt(value2[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getInt(RC_Array_final02()[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getInt(value3[0][3], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getInt(value3[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getInt(RC_Array_final03()[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        //IllegalArgumentException
        try {
            Array.getInt(value2, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        try {
            Array.getInt(value3, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        //NumberFormatException
        try {
            Integer i = new Integer(10);
            Array.setInt(value1, 0, i.decode("abc"));
        } catch (NumberFormatException e) {
            check++;
        }
        //ClassCastException
        RC_Array_01 rc1 = new RC_Array_01();
        try {
            Base_001 bs1 = new Base_001();
            rc1 = (RC_Array_01) bs1;
            rc1.base1[0] = 123;
        } catch (ClassCastException e) {
            if (rc1.base1[0] != 123)
                check++;
        }

        //Whether the judgment value is normal after Exception
        if (value1.length == 4 && value2.length == 3 && value3.length == 2)
            check++;
        else
            System.out.println("ErrorResult in RC_Array_Exception——2");
        //NullPointerException
        value1 = null;
        value2 = null;
        value3 = null;
        try {
            Array.getInt(value1, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getInt(value2, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getInt(value3, 1);
        } catch (NullPointerException e) {
            check++;
        }
        if (check == 19)
            return true;
        else
            return false;
    }

}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n