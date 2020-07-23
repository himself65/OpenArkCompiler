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
 * -@TestCaseID:maple/runtime/rc/optimization/RC_Array_03.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Scenario testing for RC optimization: testing various scenes of 1D, 2D, 3D Char array objects，including：
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
 *- @Source: RC_Array_03.java
 *- @ExecuteClass: RC_Array_03
 *- @ExecuteArgs:
 */

import java.lang.reflect.Array;
import java.util.Arrays;

class Base_003 {
    //Parent interface
    volatile char[] base1;
    char[][] base2;
    char[][] base21;
    char[][][] base3;
}

public class RC_Array_03 extends Base_003 {
    static int check_count = 0;
    static char[] arr1 = {'a', 'b', 'c', 'd'};
    static char[][] arr2 = {{'a', 'b', 'c', 'd'}, {'e', 'f'}, {'g'}};
    static char[][] arr21 = {{'h', 'i', 'j', 'k'}, {'l', 'm'}, {'n'}};
    static char[][][] arr3 = {arr2, arr21};

    private RC_Array_03() {
        base1 = new char[]{'a', 'b', 'c', 'd'};
        base2 = new char[][]{{'a', 'b', 'c', 'd'}, {'e', 'f'}, {'g'}};
        base21 = new char[][]{{'h', 'i', 'j', 'k'}, {'l', 'm'}, {'n'}};
        base3 = new char[][][]{{{'a', 'b', 'c', 'd'}, {'e', 'f'}, {'g'}}, {{'h', 'i', 'j', 'k'}, {'l', 'm'}, {'n'}}};
    }

    private RC_Array_03(char[] charar) {
        base1 = charar;
    }

    private RC_Array_03(char[][] chararr) {
        base2 = chararr;
    }

    private RC_Array_03(char[][][] chararrr) {
        base3 = chararrr;
    }

    private RC_Array_03(char[] charar, char[][] chararr, char[][][] chararrr) {
        base1 = charar;
        base2 = chararr;
        base3 = chararrr;
    }

    public static void main(String[] args) {
        final char[] TEST1 = {'1', '2', '3', '4'};
        final char[][] TEST2 = {TEST1, {'4', '5'}, {'6'}};
        final char[][] TEST21 = {{'4', '5', '6', '3'}, {'7', '8'}, {'9'}};
        final char[][][] TEST3 = {TEST2, TEST21};

        //Initialization check
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult char step1");

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

        //The test03 interface call, call the RC_Array_03_test01 function return value to the parameter third and judged.
        //RC_Array_03_test01,return 2D array，Call the multi-parameter constructor of RC_Array_01, and assign a value to
        //the newly created object field, and judge the result
        test03(TEST2);
        test03(arr2);
        if (TEST2.length == 3 && arr2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in step4");

        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_03() no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        //Test points: inheritance, constructor, return function call, constant, variable, do not receive return value
        RC_Array_03_test02();

        //Get an array object by returning a function call
        char[] getarr1 = RC_Array_get01();
        char[][] getarr2 = RC_Array_get02();
        char[][][] getarr3 = RC_Array_get03();
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

    private static void test01(int first, char[] second, char[][] third, char[][][] four) {
        //test01 interface call, internal initialization array, do not modify the parameter value, only judge
        char[] xyz = {'h', 'x', 'w', 'g'};
        char[][] xyz2 = {{'h', 'x', 'w', 'g'}, {'w', 'g'}, {'q'}};
        char[][][] xyz3 = {xyz2, xyz2};
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

    private static void test02(int first, char[] second, char[][] third, char[][][] four) {
        //test02 interface call, call function change to modify the parameter value and judge
        char[] xyz = {'h', 'x', 'w', 'g'};
        char[][] xyz2 = {{'h', 'x', 'w', 'g'}, {'w', 'g'}, {'q'}};
        char[][][] xyz3 = {xyz2, xyz2};
        second = (char[]) change(second, xyz);
        third = (char[][]) change(third, xyz2);
        four = (char[][][]) change(four, xyz3);
        if (second.length == 4 && third.length == 3 && four.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in test02");
    }

    private static void test03(char[][] third) {
        //The test03 interface is called, and the RC_Array_03_test01 function call is assigned as the return value to
        // the parameter third and judged.
        third = RC_Array_03_test01();
        if (third.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in test03");
    }

    private static char[] RC_Array_get01() {
        //Call the 1D array returned by RC_Array_set01
        return RC_Array_set01();
    }

    private static char[][] RC_Array_get02() {
        //Call the 2D array returned by RC_Array_set02
        return RC_Array_set02();
    }

    private static char[][][] RC_Array_get03() {
        //Call the 3D array returned by RC_Array_set03
        return RC_Array_set03();
    }

    private static char[] RC_Array_set01() {
        //return 1D array，Call the constructor of the 1D array parameter of RC_Array_set01, and assign a value to the
        // field of the newly created object, and judge the result
        char[] value1 = {'h', 'x', 'w', 'g'};
        RC_Array_03 rctest = new RC_Array_03(value1);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new char[]{'8', '0', '2', '3'};
        if (rctest.base1.length == 4)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set01");
        return rctest.base1;
    }

    private static char[][] RC_Array_set02() {
        //return 2D array，Call the constructor of the 2D array parameter of RC_Array_set02, and assign a
        // value to the domain of the newly created object, and judge the result
        char[][] value2 = {{'h', 'x', 'w', 'g'}, {'w', 'g'}, {'q'}};
        RC_Array_03 rctest = new RC_Array_03(value2);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base2 = new char[][]{{'8', '0', '2', '3'}, {'w', 'g'}, {'q'}};
        if (rctest.base2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set02");
        return rctest.base2;
    }

    private static char[][][] RC_Array_set03() {
        //return 3D array，Call the constructor of the 3D array parameter of RC_Array_set03, and assign a value to the
        // field of the newly created object, and judge the result
        char[][][] value3 = {{{'1', '2', '3', '4'}, {'4', '5'}, {'6'}}, {{'4', '5', '6', '3'}, {'7', '8'}, {'9'}}};
        RC_Array_03 rctest = new RC_Array_03(value3);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base3 = new char[][][]{{{'w', 'a', 'n', 'g'}, {'s', 'u'}, {'n'}}, {{'w', 'a', 'n', 'g'}, {'w', 'g'}, {'q'}}};
        if (rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set03");
        return rctest.base3;
    }

    private static char[][] RC_Array_03_test01() {
        //return 2D array，Call the multi-parameter constructor of RC_Array_03, and assign a value to the newly created
        // object field, and judge the result
        char[] value1 = {'h', 'x', 'w', 'g'};
        char[][] value2 = {{'h', 'x', 'w', 'g'}, {'w', 'g'}, {'q'}};
        char[][][] value3 = {value2, value2};
        RC_Array_03 rctest = new RC_Array_03(value1, value2, value3);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new char[]{'w', 'a', 'n', 'g'};
        rctest.base2 = new char[][]{{'w', 'a', 'n', 'g'}, {'w', 'g'}, {'q'}};
        rctest.base21 = new char[][]{{'w', 'a', 'n', 'g'}, {'s', 'u'}, {'n'}};
        rctest.base3 = new char[][][]{{{'w', 'a', 'n', 'g'}, {'s', 'u'}, {'n'}}, {{'w', 'a', 'n', 'g'}, {'w', 'g'}, {'q'}}};
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_03_test01");
        return rctest.base21;
    }

    private static char[] RC_Array_final01() {
        final char[] VALUE1 = {'h', 'x', 'w', 'g'};
        return VALUE1;
    }

    private static char[][] RC_Array_final02() {
        final char[][] VALUE2 = {{'h', 'x', 'w', 'g'}, {'w', 'g'}, {'q'}};
        return VALUE2;
    }

    private static char[][][] RC_Array_final03() {
        final char[][][] VALUE3 = {{{'1', '2', '3', '4'}, {'4', '5'}, {'6'}}, {{'4', '5', '6', '3'}, {'7', '8'}, {'9'}}};
        return VALUE3;
    }

    private static char[][] RC_Array_03_test02() {
        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_03 () no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        final char[] VALUE1 = RC_Array_final01();
        final char[][] VALUE2 = RC_Array_final02();
        final char[][][] VALUE3 = RC_Array_final03();
        RC_Array_03 rctest = new RC_Array_03();
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_03_test02");
        rctest.base1 = VALUE1;
        rctest.base2 = VALUE2;
        rctest.base21 = VALUE2;
        rctest.base3 = VALUE3;
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_03_test02_2");
        return VALUE2;
    }

    private static Boolean RC_Array_Exception() {
        //Exception test，exclude NullPointerException，ArrayIndexOutOfBoundsException and so on
        int check = 0;
        char[] value1 = RC_Array_final01();
        char[][] value2 = RC_Array_final02();
        char[][][] value3 = RC_Array_final03();
        //Is the value as expect after the assignment?
        if (value1.length == 4 && value2.length == 3 && value3.length == 2)
            check++;
        else
            System.out.println("ErrorResult in RC_Array_Exception——1");
        //ArrayIndexOutOfBoundsException
        try {
            Array.getChar(value1, 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            value1[5] = 'e';
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.setChar(value1, 5, '1');
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getChar(RC_Array_final01(), 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getChar(value2[5], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getChar(value2[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getChar(RC_Array_final02()[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getChar(value3[0][3], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getChar(value3[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.getChar(RC_Array_final03()[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        //IllegalArgumentException
        try {
            Array.getChar(value2, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        try {
            Array.getChar(value3, 1);
        } catch (IllegalArgumentException e) {
            check++;
        }
        //ClassCastException
        RC_Array_03 rc1 = new RC_Array_03();
        try {
            Base_003 bs1 = new Base_003();
            rc1 = (RC_Array_03) bs1;
            rc1.base1[0] = 'p';
        } catch (ClassCastException e) {
            if (rc1.base1[0] != 'p')
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
            Array.getChar(value1, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getChar(value2, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.getChar(value3, 1);
        } catch (NullPointerException e) {
            check++;
        }
        if (check == 18)
            return true;
        else
            return false;
    }

}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n