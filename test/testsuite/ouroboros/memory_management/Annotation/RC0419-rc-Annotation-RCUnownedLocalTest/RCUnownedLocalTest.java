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
 * -@TestCaseID:maple/runtime/rc/annotation/RCUnownedLocalTest
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:方法中添加@UnownedLocal,没有环泄露
 *- @Brief:functionTest
 * 方法中添加@UnownedLocal,没有环泄露
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: RCUnownedLocalTest.java
 *- @ExecuteClass: RCUnownedLocalTest
 *- @ExecuteArgs:
 */

import com.huawei.ark.annotation.UnownedLocal;

import java.util.LinkedList;
import java.util.List;

public class RCUnownedLocalTest {
    static Integer a = new Integer(1);
    static Object[] arr = new Object[]{1, 2, 3};

    @UnownedLocal
    static int method(Integer a, Object[] arr) {
        int check = 0;
        Integer c = a + a;
        if (c == 2) {
            check++;
        } else {
            check--;
        }
        for (Object array : arr) {
            //System.out.println(array);
            check++;
        }
        return check;
    }

    public static void main(String[] args) {
        int result = method(a, arr);
        if (result == 4) {
            System.out.println("ExpectResult");
        }
    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n