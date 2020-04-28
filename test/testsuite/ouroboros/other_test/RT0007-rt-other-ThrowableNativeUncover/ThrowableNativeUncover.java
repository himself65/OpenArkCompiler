/**
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
 * -@TestCaseID: natives/ThrowableNativeUncover.java
 * -@Title/Destination: ThrowableNativeUncover Methods
 * -@Brief:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ThrowableNativeUncover.java
 * -@ExecuteClass: ThrowableNativeUncover
 * -@ExecuteArgs:
 */

public class ThrowableNativeUncover {
    private static int res = 99;

    public static void main(String[] args) {
        ThrowableDemo1();
    }



    public static void ThrowableDemo1() {
        int result = 2;
        Throwable throwable = new Throwable();
        test(throwable);
        test1(throwable);
        if (result == 2 && res == 95) {
            res = 0;
        }
        System.out.println(res);
    }

    /**
     * private static native Object nativeFillInStackTrace();
     * @param throwable
     * @return
     */
    public static boolean test(Throwable throwable) {

        try {
            Throwable throwable2 = throwable.fillInStackTrace();//nativeFillInStackTrace() called by fillInStackTrace();
            if (throwable2.toString().equals("java.lang.Throwable")) {
                //System.out.println(throwable2.toString());
                ThrowableNativeUncover.res = ThrowableNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();

        }
        return true;
    }


    /**
     * private static native StackTraceElement[] nativeGetStackTrace(Object stackState);
     * @param throwable
     * @return
     */
    public static boolean test1(Throwable throwable) {
        try {
            StackTraceElement[] stackTraceElements = throwable.getStackTrace();//nativeGetStackTrace() called by getStackTrace();
            if (stackTraceElements.length == 3 && stackTraceElements.getClass().toString().equals("class [Ljava.lang.StackTraceElement;")) {
                //System.out.println(stackTraceElements.length);
                //System.out.println(stackTraceElements.getClass().toString());
                ThrowableNativeUncover.res = ThrowableNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return true;

    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n