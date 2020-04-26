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
 * -@TestCaseID: StringGetBytesCharsetTest.java
 * -@TestCaseName: Test String method: public byte[] getBytes(Charset charset).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: charset is Charset in normal.
 * -#step3: Test method getBytes(Charset charset).
 * -#step4: Check the return byte[] is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringGetBytesCharsetTest.java
 * -@ExecuteClass: StringGetBytesCharsetTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;

public class StringGetBytesCharsetTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringGetBytesCharsetTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringGetBytesCharsetTest_1() throws UnsupportedEncodingException {
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

    private static void test(String str) throws UnsupportedEncodingException {
        Charset charset = Charset.defaultCharset();
        byte[] test1 = str.getBytes(charset);
        for (int i = 0; i < test1.length; i++) {
            System.out.println(test1[i]);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 113\s*119\s*101\s*114\s*116\s*121\s*117\s*105\s*111\s*112\s*123\s*125\s*91\s*93\s*92\s*124\s*97\s*115\s*100\s*102\s*103\s*104\s*106\s*107\s*108\s*59\s*58\s*39\s*34\s*122\s*120\s*99\s*118\s*98\s*110\s*109\s*44\s*46\s*60\s*62\s*47\s*63\s*126\s*96\s*49\s*50\s*51\s*52\s*53\s*54\s*55\s*56\s*57\s*48\s*45\s*61\s*33\s*64\s*35\s*36\s*37\s*94\s*38\s*42\s*40\s*41\s*95\s*43\s*32\s*65\s*83\s*68\s*70\s*71\s*72\s*74\s*75\s*76\s*81\s*87\s*69\s*82\s*84\s*89\s*85\s*73\s*79\s*80\s*90\s*88\s*67\s*86\s*66\s*78\s*77\s*48\s*120\s*57\s*54\s*32\s*64\s*33\s*46\s*38\s*37\s*97\s*98\s*99\s*49\s*50\s*51\s*113\s*119\s*101\s*114\s*116\s*121\s*117\s*105\s*111\s*112\s*123\s*125\s*91\s*93\s*92\s*124\s*97\s*115\s*100\s*102\s*103\s*104\s*106\s*107\s*108\s*59\s*58\s*39\s*34\s*122\s*120\s*99\s*118\s*98\s*110\s*109\s*44\s*46\s*60\s*62\s*47\s*63\s*126\s*96\s*49\s*50\s*51\s*52\s*53\s*54\s*55\s*56\s*57\s*48\s*45\s*61\s*33\s*64\s*35\s*36\s*37\s*94\s*38\s*42\s*40\s*41\s*95\s*43\s*32\s*65\s*83\s*68\s*70\s*71\s*72\s*74\s*75\s*76\s*81\s*87\s*69\s*82\s*84\s*89\s*85\s*73\s*79\s*80\s*90\s*88\s*67\s*86\s*66\s*78\s*77\s*48\s*120\s*57\s*54\s*32\s*64\s*33\s*46\s*38\s*37\s*97\s*98\s*99\s*49\s*50\s*51\s*0