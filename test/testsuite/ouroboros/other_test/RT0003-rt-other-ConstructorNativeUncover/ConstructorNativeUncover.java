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
 * -@TestCaseID: reflect.natives/ConstructorNativeUncover.java
 * -@Title/Destination: ConstructorNativeUncover Methods
 * -@Brief:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ConstructorNativeUncover.java
 * -@ExecuteClass: ConstructorNativeUncover
 * -@ExecuteArgs:
 */

import java.lang.reflect.Constructor;
import java.util.Arrays;

public class ConstructorNativeUncover {

    private static int res = 99;

    public ConstructorNativeUncover() {
        super();
        // TODO Auto-generated constructor stub
    }

    public static void main(String[] args) {
        int result = 2;
        ConstructorDemo1();
        if (result == 2 && res == 97) {
            res = 0;
        }
        System.out.println(res);
    }


    public static void ConstructorDemo1() {
        ConstructorNativeUncover constructorNativeUncover = new ConstructorNativeUncover();
        test(constructorNativeUncover);
    }


    /**
     * public native Class<?>[] getExceptionTypes();
     * @param constructorNativeUncover
     * @return
     */
    public static boolean test(ConstructorNativeUncover constructorNativeUncover) {
        try {
            Class class1 = constructorNativeUncover.getClass();
            Constructor constructor = class1.getConstructor(new Class[] {});
            Class[] classes = constructor.getExceptionTypes();
            if (classes.length == 0 && classes.getClass().toString().equals("class [Ljava.lang.Class;")) {
//                System.out.println(classes.getClass().toString());
//                System.out.println(classes.length);
                ConstructorNativeUncover.res = ConstructorNativeUncover.res - 2;
            }
        } catch(NoSuchMethodException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n