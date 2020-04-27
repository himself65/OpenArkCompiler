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
 * -@TestCaseID:maple/runtime/rc/optimization/RC_Array_02.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Scenario testing for RC optimization: testing various scenes of 1D, 2D, 3D String array objects，including：
 *                   1.Parameter modification / parameter has not been modified
 *                   2.final、static、literial
 *                   3.As a constructor fun
 *                   4.Function call
 *                   5.Object Passing
 *                   6.return constant; variable; function call
 *                   7.Inherited as a parent class; child class inherits the parent class; interface call;
 *                   8.Exception
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RC_Array_02.java
 *- @ExecuteClass: RC_Array_02
 *- @ExecuteArgs:
 */

import java.lang.reflect.Array;
import java.util.Arrays;
import java.util.regex.PatternSyntaxException;

class Base_002 {
    //Parent interface
    volatile String[] base1;
    String[][] base2;
    String[][] base21;
    String[][][] base3;
}

public class RC_Array_02 extends Base_002 {
    static int check_count = 0;
    static String[] arr1 = {"10", "20", "30", "40"};
    static String[][] arr2 = {{"10", "20", "30", "40"}, {"40", "50"}, {"60"}};
    static String[][] arr21 = {{"40", "50", "60", "30"}, {"70", "80"}, {"90"}};
    static String[][][] arr3 = {arr2, arr21};
    //literial type data
    static String literial_v = "abc";
    static String[] arstr1 = {literial_v, literial_v, literial_v, literial_v};
    static String[][] arstr2 = {{"abc", "abc", "abc", "abc"}, {"abc", "abc"}, {literial_v}};
    static String[][][] arstr3 = {arstr2, arstr2};

    private RC_Array_02() {
        base1 = new String[]{"10", "20", "30", "40"};
        base2 = new String[][]{{"10", "20", "30", "40"}, {"40", "50"}, {"60"}};
        base21 = new String[][]{{"40", "50", "60", "30"}, {"70", "80"}, {"90"}};
        base3 = new String[][][]{{{"10", "20", "30", "40"}, {"40", "50"}, {"60"}}, {{"40", "50", "60", "30"}, {"70", "80"}, {"90"}}};
    }

    private RC_Array_02(String[] Stringar) {
        base1 = Stringar;
    }

    private RC_Array_02(String[][] Stringarr) {
        base2 = Stringarr;
    }

    private RC_Array_02(String[][][] Stringarrr) {
        base3 = Stringarrr;
    }

    private RC_Array_02(String[] Stringar, String[][] Stringarr, String[][][] Stringarrr) {
        base1 = Stringar;
        base2 = Stringarr;
        base3 = Stringarrr;
    }

    public static void main(String[] args) {
        final String[] TEST1 = {"10", "20", "30", "40"};
        final String[][] TEST2 = {TEST1, {"40", "50"}, {"60"}};
        final String[][] TEST21 = {{"40", "50", "60", "30"}, {"70", "80"}, {"90"}};
        final String[][][] TEST3 = {TEST2, TEST21};
        //literial type data
        final String literial_v = "abc";
        final String[] arstrr1 = {literial_v, literial_v, literial_v, literial_v};
        final String[][] arstrr2 = {{"abc", "abc", "abc", "abc"}, {"abc", "abc"}, {literial_v}};
        final String[][][] arstrr3 = {arstrr2, arstrr2};

        //Initialization check
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2
                && arstrr1.length == 4 && arstrr2.length == 3 && arstrr3.length == 2
                && arstr1.length == 4 && arstr2.length == 3 && arstr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult String step1");

        //test01 interface call, internal initialization array, do not modify the parameter value, only judge
        test01(4, TEST1, TEST2, TEST3);
        test01(4, arr1, arr2, arr3);
        test01(4, arstr1, arstr2, arstr3);
        test01(4, arstrr1, arstrr2, arstrr3);

        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2
                && arstrr1.length == 4 && arstrr2.length == 3 && arstrr3.length == 2
                && arstr1.length == 4 && arstr2.length == 3 && arstr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step2");

        //test02 interface call, call function change to modify the parameter value and judge
        test02(4, TEST1, TEST2, TEST3);
        test02(4, arr1, arr2, arr3);
        test02(4, arstr1, arstr2, arstr3);
        test02(4, arstrr1, arstrr2, arstrr3);
        if (TEST1.length == 4 && TEST2.length == 3 && TEST3.length == 2
                && arr1.length == 4 && arr2.length == 3 && arr3.length == 2
                && arstrr1.length == 4 && arstrr2.length == 3 && arstrr3.length == 2
                && arstr1.length == 4 && arstr2.length == 3 && arstr3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in step3");

        //The test03 interface call, call the RC_Array_02_test01 function return value to the parameter third and judged.
        //RC_Array_02_test01,return 2D array，Call the multi-parameter constructor of RC_Array_01, and assign a value to
        //the newly created object field, and judge the result
        test03(TEST2);
        test03(arr2);
        if (TEST2.length == 3 && arr2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in step4");

        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_02() no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        //Test points: inheritance, constructor, return function call, constant, variable, do not receive return value
        RC_Array_02_test02();

        //Get an array object by returning a function call
        String[] getarr1 = RC_Array_get01();
        String[][] getarr2 = RC_Array_get02();
        String[][][] getarr3 = RC_Array_get03();
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
        if (check_count == 28)
            System.out.println("ExpectResult");

    }

    private static void test01(int first, String[] second, String[][] third, String[][][] four) {
        //test01 interface call, internal initialization array, do not modify the parameter value, only judge
        String[] xyz = {"23", "24", "25", "26"};
        String[][] xyz2 = {{"23", "24", "25", "26"}, {"23", "24"}, {"23"}};
        String[][][] xyz3 = {xyz2, xyz2};
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

    private static void test02(int first, String[] second, String[][] third, String[][][] four) {
        //test02 interface call, call function change to modify the parameter value and judge
        String[] xyz = {"23", "24", "25", "26"};
        String[][] xyz2 = {{"23", "24", "25", "26"}, {"23", "24"}, {"23"}};
        String[][][] xyz3 = {xyz2, xyz2};
        second = (String[]) change(second, xyz);
        third = (String[][]) change(third, xyz2);
        four = (String[][][]) change(four, xyz3);
        if (second.length == 4 && third.length == 3 && four.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in test02");
    }

    private static void test03(String[][] third) {
        //The test03 interface is called, and the RC_Array_02_test01 function call is assigned as the return value to
        // the parameter third and judged.
        third = RC_Array_02_test01();
        if (third.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in test03");
    }

    private static String[] RC_Array_get01() {
        //Call the 1D array returned by RC_Array_set01
        return RC_Array_set01();
    }

    private static String[][] RC_Array_get02() {
        //Call the 2D array returned by RC_Array_set02
        return RC_Array_set02();
    }

    private static String[][][] RC_Array_get03() {
        //Call the 3D array returned by RC_Array_set03
        return RC_Array_set03();
    }

    private static String[] RC_Array_set01() {
        //return 1D array，Call the constructor of the 1D array parameter of RC_Array_set01, and assign a value to the
        // field of the newly created object, and judge the result
        String[] value1 = {"23", "24", "25", "26"};
        RC_Array_02 rctest = new RC_Array_02(value1);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new String[]{"88", "10", "02", "11"};
        if (rctest.base1.length == 4)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set01");
        return rctest.base1;
    }

    private static String[][] RC_Array_set02() {
        //return 2D array，Call the constructor of the 2D array parameter of RC_Array_set02, and assign a
        // value to the domain of the newly created object, and judge the result
        String[][] value2 = {{"23", "24", "25", "26"}, {"23", "24"}, {"23"}};
        RC_Array_02 rctest = new RC_Array_02(value2);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, null) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base2 = new String[][]{{"88", "10", "02", "11"}, {"10", "92"}, {"16"}};
        if (rctest.base2.length == 3)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set02");
        return rctest.base2;
    }

    private static String[][][] RC_Array_set03() {
        //return 3D array，Call the constructor of the 3D array parameter of RC_Array_set03, and assign a value to the
        // field of the newly created object, and judge the result
        String[][][] value3 = {{{"10", "20", "30", "40"}, {"40", "50"}, {"60"}}, {{"40", "50", "60", "30"}, {"70", "80"}, {"90"}}};
        RC_Array_02 rctest = new RC_Array_02(value3);
        if (Arrays.equals(rctest.base1, null) && Arrays.equals(rctest.base2, null)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base3 = new String[][][]{{{"88", "10", "02", "11"}, {"10", "92"}, {"12"}}, {{"88", "10", "02", "11"}, {"10", "92"}, {"16"}}};
        if (rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_set03");
        return rctest.base3;
    }

    private static String[][] RC_Array_02_test01() {
        //return 2D array，Call the multi-parameter constructor of RC_Array_02, and assign a value to the newly created
        // object field, and judge the result
        String[] value1 = {"23", "24", "25", "26"};
        String[][] value2 = {{"23", "24", "25", "26"}, {"23", "24"}, {"23"}};
        String[][][] value3 = {value2, value2};
        RC_Array_02 rctest = new RC_Array_02(value1, value2, value3);
        if (Arrays.equals(rctest.base1, value1) && Arrays.equals(rctest.base2, value2)
                && Arrays.equals(rctest.base3, value3) && Arrays.equals(rctest.base21, null))
            check_count++;
        rctest.base1 = new String[]{"88", "10", "02", "11"};
        rctest.base2 = new String[][]{{"88", "10", "02", "11"}, {"10", "92"}, {"16"}};
        rctest.base21 = new String[][]{{"88", "10", "02", "11"}, {"10", "92"}, {"12"}};
        rctest.base3 = new String[][][]{{{"88", "10", "02", "11"}, {"10", "92"}, {"12"}}, {{"88", "10", "02", "11"}, {"10", "92"}, {"16"}}};
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_02_test01");
        return rctest.base21;
    }

    private static String[] RC_Array_final01() {
        final String[] VALUE1 = {"23", "24", "25", "26"};
        return VALUE1;
    }

    private static String[][] RC_Array_final02() {
        final String[][] VALUE2 = {{"23", "24", "25", "26"}, {"23", "24"}, {"23"}};
        return VALUE2;
    }

    private static String[][][] RC_Array_final03() {
        final String[][][] VALUE3 = {{{"10", "20", "30", "40"}, {"40", "50"}, {"60"}}, {{"40", "50", "60", "30"}, {"70", "80"}, {"90"}}};
        return VALUE3;
    }

    private static String[][] RC_Array_02_test02() {
        //The calling function returns a constant value as the assignment of the constant of this function.
        //Call RC_Array_02 () no argument construction method, initialize the variable of the parent class, and assign
        // a value to the domain of the newly created object, and judge the result
        final String[] VALUE1 = RC_Array_final01();
        final String[][] VALUE2 = RC_Array_final02();
        final String[][][] VALUE3 = RC_Array_final03();
        RC_Array_02 rctest = new RC_Array_02();
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_02_test02");
        rctest.base1 = VALUE1;
        rctest.base2 = VALUE2;
        rctest.base21 = VALUE2;
        rctest.base3 = VALUE3;
        if (rctest.base1.length == 4 && rctest.base2.length == 3 && rctest.base21.length == 3 && rctest.base3.length == 2)
            check_count++;
        else
            System.out.println("ErrorResult in RC_Array_02_test02_2");
        return VALUE2;
    }

    private static Boolean RC_Array_Exception() {
        //Exception test，exclude NullPointerException，ArrayIndexOutOfBoundsException and so on
        int check = 0;
        String[] value1 = RC_Array_final01();
        String[][] value2 = RC_Array_final02();
        String[][][] value3 = RC_Array_final03();
        //Is the value as expect after the assignment?
        if (value1.length == 4 && value2.length == 3 && value3.length == 2)
            check++;
        else
            System.out.println("ErrorResult in RC_Array_Exception——1");
        //ArrayIndexOutOfBoundsException
        try {
            Array.get(value1, 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            value1[5] = "error";
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.set(value1, 5, "10");
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.get(RC_Array_final01(), 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.get(value2[5], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.get(value2[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.get(RC_Array_final02()[0], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.get(value3[0][3], 0);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.get(value3[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }
        try {
            Array.get(RC_Array_final03()[0][1], 5);
        } catch (ArrayIndexOutOfBoundsException e) {
            check++;
        }

        //IndexOutOfBoundsException
        try {
            char[] ch1 = {'h', 'x', 'c', 't'};
            Array.set(RC_Array_final03()[0][0], 0, String.valueOf(ch1, 5, 2));
        } catch (IndexOutOfBoundsException e) {
            check++;
        }
        //ClassCastException
        RC_Array_02 rc1 = new RC_Array_02();
        try {
            Base_002 bs1 = new Base_002();
            rc1 = (RC_Array_02) bs1;
            rc1.base1[0] = "kitty";
        } catch (ClassCastException e) {
            if (rc1.base1[0] != "kitty")
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
            Array.get(value1, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.get(value2, 1);
        } catch (NullPointerException e) {
            check++;
        }
        try {
            Array.get(value3, 1);
        } catch (NullPointerException e) {
            check++;
        }
        //System.out.println(check);
        if (check == 17)
            return true;
        else
            return false;
    }

}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n