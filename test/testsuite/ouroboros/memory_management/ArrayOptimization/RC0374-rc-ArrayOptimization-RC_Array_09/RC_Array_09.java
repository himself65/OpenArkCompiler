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
 * -@TestCaseID:maple/runtime/rc/optimization/RC_Array_09.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Scenario testing for RC optimization: testing various scenes of 1D, 2D, 3D double array objects，including：
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
 *- @Source: RC_Array_09.java
 *- @ExecuteClass: RC_Array_09
 *- @ExecuteArgs:
 */

import java.lang.reflect.Array;
import java.util.Arrays;

class Base_009 {
    //Parent interface
    volatile double[] base1;
    double[][] base2;
    double[][] base21;
    double[][][] base3;
}

public class RC_Array_09 extends Base_009 {
    static int check_count = 0;
    static double[] arr1 = {1.1f, 2.2f, 3.3f, 4.4f};
    static double[][] arr2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f}};
    static double[][] arr21 = {{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}};
    static double[][][] arr3 = {arr2, arr21};

    private RC_Array_09() {
        base1 = new double[]{1.1f, 2.2f, 3.3f, 4.4f};
        base2 = new double[][]{{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f}};
        base21 = new double[][]{{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}};
        base3 = new double[][][]{{{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f}}, {{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}}};
    }

    private RC_Array_09(double[] doublear) {
        base1 = doublear;
    }

    private RC_Array_09(double[][] doublearr) {
        base2 = doublearr;
    }

    private RC_Array_09(double[][][] doublearrr) {
        base3 = doublearrr;
    }

    private RC_Array_09(double[] doublear, double[][] doublearr, double[][][] doublearrr) {
        base1 = doublear;
        base2 = doublearr;
        base3 = doublearrr;
    }

    public static void main(String[] args) {
        final double[] TEST1 = {1.1f, 2.2f, 3.3f, 4.4f};
        final double[][] TEST2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f}};
        final double[][] TEST21 = {{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}};
        final double[][][] TEST3 = {TEST2, TEST21};

        //Initialization check
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult double step1");

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
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step3");

        //The test03 interface call, call the RC_Array_09_test01 function return value to the parameter third and judged.
        //RC_Array_09_test01,return 2D array，Call the multi-parameter constructor of RC_Array_01, and assign a value to
        //the newly created object field, and judge the result
        test03(TEST2);
        test03(arr2);
        if (TEST2.length == 3 && arr2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in step4");

        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_09() no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        //Test points: inheritance, constructor, return function call, constant, variable, do not receive return value
        RC_Array_09_test02();

        //Get an array object by returning a function call
        double[] getarr1 = RC_Array_get01();
        double[][] getarr2 = RC_Array_get02();
        double[][][] getarr3 = RC_Array_get03();
        if (getarr1.length == 4 && getarr2.length == 3 && getarr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step5");

        //Exception testing
        boolean ret = RC_Array_Exception();
        if (ret == true)
            check_count++;
        else
            System.out.println("RC_Array_Exception ErrorResult");

        //Result judgment
        //System.out.println(check_count);
        if (check_count == 24)
            System.out.println("ExpectResult");

    }

    private static void test01(int first, double[] second, double[][] third, double[][][] four) {
        //test01 interface call, internal initialization array, do not modify the parameter value, only judge
        double[] xyz = {8.5f, 8.3f, 1.2f, 0.5f};
        double[][] xyz2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        double[][][] xyz3 = {xyz2, xyz2};
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

    private static void test02(int first, double[] second, double[][] third, double[][][] four) {
        //test02 interface call, call function change to modify the parameter value and judge
        double[] xyz = {8.5f, 8.3f, 1.2f, 0.5f};
        double[][] xyz2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        double[][][] xyz3 = {xyz2, xyz2};
        second = (double[]) change(second, xyz);
        third = (double[][]) change(third, xyz2);
        four = (double[][][]) change(four, xyz3);
        if (second.length == 4 && third.length == 3 && four.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in test02");
    }

    private static void test03(double[][] third) {
        //The test03 interface is called, and the RC_Array_09_test01 function call is assigned as the return value to
        // the parameter third and judged.
        third = RC_Array_09_test01();
        if (third.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in test03");
    }

    private static double[] RC_Array_get01() {
        //Call the 1D array returned by RC_Array_set01
        return RC_Array_set01();
    }

    private static double[][] RC_Array_get02() {
        //Call the 2D array returned by RC_Array_set02
        return RC_Array_set02();
    }

    private static double[][][] RC_Array_get03() {
        //Call the 3D array returned by RC_Array_set03
        return RC_Array_set03();
    }

    private static double[] RC_Array_set01() {
        //return 1D array，Call the constructor of the 1D array parameter of RC_Array_set01, and assign a value to the
        // field of the newly created object, and judge the result
        double[] value1 = {9.2f, 2.2f, 8.8f, 8.8f};
        RC_Array_09 rctest = new RC_Array_09(value1);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new double[]{1.1f, 2.2f, 3.3f, 4.4f};
        if (rctest.base1.length == 4)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set01");
        return rctest.base1;
    }

    private static double[][] RC_Array_set02() {
        //return 2D array，Call the constructor of the 2D array parameter of RC_Array_set02, and assign a
        // value to the domain of the newly created object, and judge the result
        double[][] value2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        RC_Array_09 rctest = new RC_Array_09(value2);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base2 = new double[][]{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}};
        if (rctest.base2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set02");
        return rctest.base2;
    }

    private static double[][][] RC_Array_set03() {
        //return 3D array，Call the constructor of the 3D array parameter of RC_Array_set03, and assign a value to the
        // field of the newly created object, and judge the result
        double[][][] value3 = {{{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}}, {{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}};
        RC_Array_09 rctest = new RC_Array_09(value3);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base3 = new double[][][]{{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}, {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}}};
        if (rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set03");
        return rctest.base3;
    }

    private static double[][] RC_Array_09_test01() {
        //return 2D array，Call the multi-parameter constructor of RC_Array_09, and assign a value to the newly created
        // object field, and judge the result
        double[] value1 = {1.1f, 2.2f, 3.3f, 4.4f};
        double[][] value2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        double[][][] value3 = {value2, value2};
        RC_Array_09 rctest = new RC_Array_09(value1, value2, value3);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new double[]{9.2f, 2.2f, 8.8f, 8.8f};
        rctest.base2 = new double[][]{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}};
        rctest.base21 = new double[][]{{6, 6, 6, 5}, {1.3f, 1.1f}, {0.5f}};
        rctest.base3 = new double[][][]{{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}, {{6, 6, 6, 5}, {1.3f, 1.1f}, {0.5f}}};
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_09_test01");
        return rctest.base21;
    }

    private static double[] RC_Array_final01() {
        final double[] VALUE1 = {1.1f, 2.2f, 3.3f, 4.4f};
        return VALUE1;
    }

    private static double[][] RC_Array_final02() {
        final double[][] VALUE2 = {{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}};
        return VALUE2;
    }

    private static double[][][] RC_Array_final03() {
        final double[][][] VALUE3 = {{{6.4f, 6.1f, 6.3f, 5.5f}, {1.3f, 1.1f}, {0.5f}}, {{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}};
        return VALUE3;
    }

    private static double[][] RC_Array_09_test02() {
        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_09() no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        final double[] VALUE1 = RC_Array_final01();
        final double[][] VALUE2 = RC_Array_final02();
        final double[][][] VALUE3 = RC_Array_final03();
        RC_Array_09 rctest = new RC_Array_09();
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_09_test02");
        rctest.base1 = VALUE1;
        rctest.base2 = VALUE2;
        rctest.base21 = VALUE2;
        rctest.base3 = VALUE3;
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_09_test02_2");
        return VALUE2;
    }

    private static boolean RC_Array_Exception() {
        //Exception test，exclude NullPointerException，ArrayIndexOutOfBoundsException and so on
        int check = 0;
        double[] value1 = RC_Array_final01();
        double[][] value2 = RC_Array_final02();
        double[][][] value3 = RC_Array_final03();

        //Is the value as expect after the assignment?
        if (value1.length == 4 && value2.length == 3 && value3.length == 2)
            check++;
        else
            System.out.println("ErrorResult in RC_Array_Exception——1");

        //ArrayIndexOutOfBoundsException
        try {
            Array.getDouble(value1, 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            value1[5] = 10.16f;
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.setDouble(value1, 5, 12.5f);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getDouble(value2[5], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getDouble(value2[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getDouble(value3[0][3], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getDouble(value3[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        //IllegalArgumentException
        try {
            Array.getDouble(value2, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        try {
            Array.getDouble(value3, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        //ClassCastException
        RC_Array_09 rc1 = new RC_Array_09();
        try {
            Base_009 bs1 = new Base_009();
            rc1 = (RC_Array_09) bs1;
            rc1.base1[0] = 8.9f;
        } catch (ClassCastException e) {
            if (rc1.base1[0] != 8.9f)
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
            Array.getDouble(value1, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getDouble(value2, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getDouble(value3, 1);
        } catch (NullPointerException e) {
            check++;
        }

        //System.out.println(check);
        if (check == 15)
            return true;
        else
            return false;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n