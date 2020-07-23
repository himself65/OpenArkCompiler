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
 * -@TestCaseID: ModifierExObjecthashCode.java
 * -@TestCaseName: Exception in reflect Modifier: int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create Object1 by new Modifier(), Object2 is given by Object1
 * -#step2: Create Object3 by new Modifier().
 * -#step3: Check hashCode of the Object1 and Object2 is equal, check hashCode of the Object1 and Object3 is not equal.
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ModifierExObjecthashCode.java
 * -@ExecuteClass: ModifierExObjecthashCode
 * -@ExecuteArgs:
 */

import java.lang.reflect.Modifier;

public class ModifierExObjecthashCode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ModifierExObjecthashCode().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = modifierExObjecthashCode1();
        } catch (Exception e) {
            ModifierExObjecthashCode.res = ModifierExObjecthashCode.res - 20;
        }


        if (result == 4 && ModifierExObjecthashCode.res == 89) {
            result = 0;
        }

        return result;
    }


    private int modifierExObjecthashCode1() {
        int result1 = 4; /*STATUS_FAILED*/
        // int hashCode()

        Modifier mf0 = new Modifier();
        Modifier mf1 = mf0;
        Modifier mf2 = new Modifier();

        int px0 = mf0.hashCode();
        int px1 = mf1.hashCode();
        int px2 = mf2.hashCode();

        if (px0 == px1 && px0 != px2) {
            ModifierExObjecthashCode.res = ModifierExObjecthashCode.res - 10;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n