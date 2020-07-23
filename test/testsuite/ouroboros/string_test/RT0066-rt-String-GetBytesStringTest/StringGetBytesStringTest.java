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
 * -@TestCaseID: StringGetBytesStringTest.java
 * -@TestCaseName: Test String Method: byte[] getBytes(String charsetName).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: charsetName is Charset Name String.
 * -#step3: Test method getBytes(String charsetName).
 * -#step4: Check the return byte[] is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringGetBytesStringTest.java
 * -@ExecuteClass: StringGetBytesStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

public class StringGetBytesStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringGetBytesStringTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringGetBytesStringTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123");

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str2_1);
        test(str2_2);
        test(str2_3);
    }

    private static void test(String str) {
        try {
            byte[] test1_1 = str.getBytes("UTF-8");
            for (int i = 0; i < test1_1.length; i++) {
                System.out.println(test1_1[i]);
            }
        } catch (UnsupportedEncodingException e) {
            System.out.println("Unsupported character set");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 113\n119\n101\n114\n116\n121\n117\n105\n111\n112\n123\n125\n91\n93\n92\n124\n97\n115\n100\n102\n103\n104\n106\n107\n108\n59\n58\n39\n34\n122\n120\n99\n118\n98\n110\n109\n44\n46\n60\n62\n47\n63\n126\n96\n49\n50\n51\n52\n53\n54\n55\n56\n57\n48\n45\n61\n33\n64\n35\n36\n37\n94\n38\n42\n40\n41\n95\n43\n32\n65\n83\n68\n70\n71\n72\n74\n75\n76\n81\n87\n69\n82\n84\n89\n85\n73\n79\n80\n90\n88\n67\n86\n66\n78\n77\n48\n120\n57\n54\n32\n64\n33\n46\n38\n37\n97\n98\n99\n49\n50\n51\n113\n119\n101\n114\n116\n121\n117\n105\n111\n112\n123\n125\n91\n93\n92\n124\n97\n115\n100\n102\n103\n104\n106\n107\n108\n59\n58\n39\n34\n122\n120\n99\n118\n98\n110\n109\n44\n46\n60\n62\n47\n63\n126\n96\n49\n50\n51\n52\n53\n54\n55\n56\n57\n48\n45\n61\n33\n64\n35\n36\n37\n94\n38\n42\n40\n41\n95\n43\n32\n65\n83\n68\n70\n71\n72\n74\n75\n76\n81\n87\n69\n82\n84\n89\n85\n73\n79\n80\n90\n88\n67\n86\n66\n78\n77\n48\n120\n57\n54\n32\n64\n33\n46\n38\n37\n97\n98\n99\n49\n50\n51\n0\n