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
 * -@TestCaseID:maple/runtime/rc/optimization/RC_Array_08.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Scenario testing for RC optimization: testing various scenes of 1D, 2D, 3D float array objects，including：
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
 *- @Source: RC_Array_08.java
 *- @ExecuteClass: RC_Array_08
 *- @ExecuteArgs:
 */

import java.lang.reflect.Array;
import java.util.Arrays;

class Base_008 {
    //Parent interface
    volatile float[] base1;
    float[][] base2;
    float[][] base21;
    float[][][] base3;
}

public class RC_Array_08 extends Base_008 {
    static int check_count = 0;
    static float[] arr1 = {1.1f, 2.2f, 3.3f, 4.4f};
    static float[][] arr2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f}};
    static float[][] arr21 = {{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}};
    static float[][][] arr3 = {arr2, arr21};

    private RC_Array_08() {
        base1 = new float[]{1.1f, 2.2f, 3.3f, 4.4f};
        base2 = new float[][]{{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f}};
        base21 = new float[][]{{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}};
        base3 = new float[][][]{{{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f}}, {{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}}};
    }

    private RC_Array_08(float[] floatar) {
        base1 = floatar;
    }

    private RC_Array_08(float[][] floatarr) {
        base2 = floatarr;
    }

    private RC_Array_08(float[][][] floatarrr) {
        base3 = floatarrr;
    }

    private RC_Array_08(float[] floatar, float[][] floatarr, float[][][] floatarrr) {
        base1 = floatar;
        base2 = floatarr;
        base3 = floatarrr;
    }

    public static void main(String[] args) {
        final float[] TEST1 = {1.1f, 2.2f, 3.3f, 4.4f};
        final float[][] TEST2 = {TEST1, {5.5f, 6.6f}, {7.7f, 8.8f}};
        final float[][] TEST21 = {{4.1f, 5.1f, 6.1f, 7.1f}, {8.1f, 9.1f}, {10.3f, 11.10f}};
        final float[][][] TEST3 = {TEST2, TEST21};

        //Initialization check
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult float step1");

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

        //The test03 interface call, call the RC_Array_08_test01 function return value to the parameter third and judged.
        //RC_Array_08_test01,return 2D array，Call the multi-parameter constructor of RC_Array_01, and assign a value to
        //the newly created object field, and judge the result
        test03(TEST2);
        test03(arr2);
        if (TEST2.length == 3 && arr2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in step4");

        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_08() no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        //Test points: inheritance, constructor, return function call, constant, variable, do not receive return value
        RC_Array_08_test02();

        //Get an array object by returning a function call
        float[] getarr1 = RC_Array_get01();
        float[][] getarr2 = RC_Array_get02();
        float[][][] getarr3 = RC_Array_get03();
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

    private static void test01(int first, float[] second, float[][] third, float[][][] four) {
        //test01 interface call, internal initialization array, do not modify the parameter value, only judge
        float[] xyz = {8.5f, 8.3f, 1.2f, 0.5f};
        float[][] xyz2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        float[][][] xyz3 = {xyz2, xyz2};
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

    private static void test02(int first, float[] second, float[][] third, float[][][] four) {
        //test02 interface call, call function change to modify the parameter value and judge
        float[] xyz = {8.5f, 8.3f, 1.2f, 0.5f};
        float[][] xyz2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        float[][][] xyz3 = {xyz2, xyz2};
        second = (float[]) change(second, xyz);
        third = (float[][]) change(third, xyz2);
        four = (float[][][]) change(four, xyz3);
        if (second.length == 4 && third.length == 3 && four.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in test02");
    }

    private static void test03(float[][] third) {
        //The test03 interface is called, and the RC_Array_08_test01 function call is assigned as the return value to
        // the parameter third and judged.
        third = RC_Array_08_test01();
        if (third.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in test03");
    }

    private static float[] RC_Array_get01() {
        //Call the 1D array returned by RC_Array_set01
        return RC_Array_set01();
    }

    private static float[][] RC_Array_get02() {
        //Call the 2D array returned by RC_Array_set02
        return RC_Array_set02();
    }

    private static float[][][] RC_Array_get03() {
        //Call the 3D array returned by RC_Array_set03
        return RC_Array_set03();
    }

    private static float[] RC_Array_set01() {
        //return 1D array，Call the constructor of the 1D array parameter of RC_Array_set01, and assign a value to the
        // field of the newly created object, and judge the result
        float[] value1 = {9.2f, 2.2f, 8.8f, 8.8f};
        RC_Array_08 rctest = new RC_Array_08(value1);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new float[]{1.1f, 2.2f, 3.3f, 4.4f};
        if (rctest.base1.length == 4)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set01");
        return rctest.base1;
    }

    private static float[][] RC_Array_set02() {
        //return 2D array，Call the constructor of the 2D array parameter of RC_Array_set02, and assign a
        // value to the domain of the newly created object, and judge the result
        float[][] value2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        RC_Array_08 rctest = new RC_Array_08(value2);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base2 = new float[][]{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}};
        if (rctest.base2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set02");
        return rctest.base2;
    }

    private static float[][][] RC_Array_set03() {
        //return 3D array，Call the constructor of the 3D array parameter of RC_Array_set03, and assign a value to the
        // field of the newly created object, and judge the result
        float[][][] value3 = {{{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}}, {{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}};
        RC_Array_08 rctest = new RC_Array_08(value3);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base3 = new float[][][]{{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}, {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}}};
        if (rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set03");
        return rctest.base3;
    }

    private static float[][] RC_Array_08_test01() {
        //return 2D array，Call the multi-parameter constructor of RC_Array_08, and assign a value to the newly created
        // object field, and judge the result
        float[] value1 = {1.1f, 2.2f, 3.3f, 4.4f};
        float[][] value2 = {{1.1f, 2.2f, 3.3f, 4.4f}, {1.3f, 1.1f}, {0.5f}};
        float[][][] value3 = {value2, value2};
        RC_Array_08 rctest = new RC_Array_08(value1, value2, value3);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new float[]{9.2f, 2.2f, 8.8f, 8.8f};
        rctest.base2 = new float[][]{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}};
        rctest.base21 = new float[][]{{6, 6, 6, 5}, {1.3f, 1.1f}, {0.5f}};
        rctest.base3 = new float[][][]{{{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}, {{6, 6, 6, 5}, {1.3f, 1.1f}, {0.5f}}};
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_08_test01");
        return rctest.base21;
    }

    private static float[] RC_Array_final01() {
        final float[] VALUE1 = {1.1f, 2.2f, 3.3f, 4.4f};
        return VALUE1;
    }

    private static float[][] RC_Array_final02() {
        final float[][] VALUE2 = {{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}};
        return VALUE2;
    }

    private static float[][][] RC_Array_final03() {
        final float[][][] VALUE3 = {{{6.4f, 6.1f, 6.3f, 5.5f}, {1.3f, 1.1f}, {0.5f}}, {{9.2f, 2.2f, 8.8f, 8.8f}, {1.3f, 1.1f}, {0.5f}}};
        return VALUE3;
    }

    private static float[][] RC_Array_08_test02() {
        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_08() no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        final float[] VALUE1 = RC_Array_final01();
        final float[][] VALUE2 = RC_Array_final02();
        final float[][][] VALUE3 = RC_Array_final03();
        RC_Array_08 rctest = new RC_Array_08();
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_08_test02");
        rctest.base1 = VALUE1;
        rctest.base2 = VALUE2;
        rctest.base21 = VALUE2;
        rctest.base3 = VALUE3;
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_08_test02_2");
        return VALUE2;
    }

    private static boolean RC_Array_Exception() {
        //Exception test，exclude NullPointerException，ArrayIndexOutOfBoundsException and so on
        int check = 0;
        float[] value1 = RC_Array_final01();
        float[][] value2 = RC_Array_final02();
        float[][][] value3 = RC_Array_final03();

        //Is the value as expect after the assignment?
        if (value1.length == 4 && value2.length == 3 && value3.length == 2)
            check++;
        else
            System.out.println("ErrorResult in RC_Array_Exception——1");

        //ArrayIndexOutOfBoundsException
        try {
            Array.getFloat(value1, 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            value1[5] = 88.10f;
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.setFloat(value1, 5, 12.5f);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getFloat(RC_Array_final01(), 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getFloat(value2[5], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getFloat(value2[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getFloat(RC_Array_final02()[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getFloat(value3[0][3], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getFloat(value3[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getFloat(RC_Array_final03()[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        //IllegalArgumentException
        try {
            Array.getFloat(value2, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        try {
            Array.getFloat(value3, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        //ClassCastException
        RC_Array_08 rc1 = new RC_Array_08();
        try {
            Base_008 bs1 = new Base_008();
            rc1 = (RC_Array_08) bs1;
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
            Array.getFloat(value1, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getFloat(value2, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getFloat(value3, 1);
        } catch (NullPointerException e) {
            check++;
        }
        //System.out.println(check);
        if (check == 18)
            return true;
        else
            return false;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n