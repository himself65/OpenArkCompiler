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
 * -@TestCaseID: Maple_CompilerOptimization_LiverangeSubsumeRCTest03
 *- @TestCaseName: LiverangeSubsumeRCTest03
 *- @TestCaseType: Function Testing
 *- @RequirementName: Liverange subsumeRC优化
 *- @Brief:当确定一个对象的在某个生存区间内的存活状态时，去掉这个区间内的此对象的RC操作
 *  -#step1: 创建一个1个数组，先后用array1、array2、array3指针指向了它;
 *  -#step2: str1、str2和str3同时指向一个字符串，step1和step2里他们的生命周期都是按照这个流程来的：
 *  incref(array1)
 *  array2 = array1   incref优化掉
 *  array3 = array2  incref优化掉
 *  decref(array2)   优化掉
 *  decref(array3)   优化掉
 *  decref(array1)
 *  -#step3:
 *  校验中间文件LiverangeSubsumeRCTest01.VtableImpl.mpl中无IncRef
 *- @Expect:falsefalseExpectResult\n
 *- @Priority: High
 *- @Source: LiverangeSubsumeRCTest03.java
 *- @ExecuteClass: LiverangeSubsumeRCTest03
 *- @ExecuteArgs:
 */

public class LiverangeSubsumeRCTest03 {
    public static void main(String[] args) {
        int[] array1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int[] array2 = array1;
        int[] array3 = array2;
        String str1 = "ExpectResult";
        String str2 = str1;
        String str3 = str2;
        boolean check = String.valueOf(array2[0]).equals(str2); // false
        System.out.print(check);
        check = String.valueOf(array3[1]) == str3;
        System.out.print(check); // false
        if (array1.length == 10) {
            System.out.println(str1);
        } else {
            System.out.print(str1); // 错误输出，无换行符
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan falsefalseExpectResult\n