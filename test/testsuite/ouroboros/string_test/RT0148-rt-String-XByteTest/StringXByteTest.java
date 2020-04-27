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
 * -@TestCaseID: StringXByteTest.java
 * -@TestCaseName: Test String(byte[] bytes), bytes cover 1、2、3、4 byte.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create string value and get the random string byte by string value.
 * -#step2: Get byte array by random string cover with 1、2、3、4 byte.
 * -#step3: Test constructor String(byte[] bytes), check the byte array change to string is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringXByteTest.java
 * -@ExecuteClass: StringXByteTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.*;
import java.util.Random;

public class StringXByteTest {
    static int seed = 20;
    static Random random = new Random(seed);

    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    public static int run(String[] args, PrintStream out) {
        int result = 2;

        try {
            result = StringXByteTest1();
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (result == 1 ) {
            result = 0;
        }
        return result;
    }

    public static int StringXByteTest1() {
        String string_byte1 = getByteString(1);
        String string_byte2 = getByteString(2);
        String string_byte3 = getByteString(3);
        String string_byte4 = getByteString(4);

        byte[] by = new byte[]{(byte) 0x59,(byte) 0xCF,(byte) 0xB9,(byte) 0xE2,(byte) 0xA2,(byte) 0xA4,
                (byte) 0xF3,(byte) 0x90,(byte) 0xA2,(byte) 0x85};

        if ((string_byte1+string_byte2+string_byte3+string_byte4).equals(new String(by))) {
            return 1;
        }
        return 3;
    }

    public static String getByteString(int lengthIn) {
        String strCd = "CD";
        String str0f = "0123456789ABCDEF";
        String str8b = "89AB";
        String str07 = "01234567";
        String hexString = "0123456789ABCDEF";
        StringBuffer byte5= new StringBuffer();

        if (lengthIn == 1) {
            // 1Byte: 0-7|0-F
            byte5.append(str07.charAt( random.nextInt(8))); // 0-7
            byte5.append(str0f.charAt( random.nextInt(16))); // 0-F
        }else if (lengthIn == 2) {
            // 2Byte: C-D|0-F   8-B|0-F
            byte5.append(strCd.charAt(random.nextInt(2)));
            byte5.append(str0f.charAt(random.nextInt(16)));
            byte5.append(str8b.charAt(random.nextInt(4)));
            byte5.append(str0f.charAt(random.nextInt(16)));
        }else if (lengthIn == 3) {
            // 3Byte: E|0-F  8-B|0-F 8-B|0-F
            byte5.append("E");
            byte5.append(str0f.charAt(random.nextInt(16)));
            byte5.append(str8b.charAt(random.nextInt(4)));
            byte5.append(str0f.charAt(random.nextInt(16)));
            byte5.append(str8b.charAt(random.nextInt(4)));
            byte5.append(str0f.charAt(random.nextInt(16)));
        }else if (lengthIn == 4) {
            // 4Byte: F|0-7 8-B|0-F 8-B|0-F 8-B|0-F
            byte5.append("F");
            byte5.append(str07.charAt(random.nextInt(8)));
            byte5.append(str8b.charAt(random.nextInt(4)));
            byte5.append(str0f.charAt(random.nextInt(16)));
            byte5.append(str8b.charAt(random.nextInt(4)));
            byte5.append(str0f.charAt(random.nextInt(16)));
            byte5.append(str8b.charAt(random.nextInt(4)));
            byte5.append(str0f.charAt(random.nextInt(16)));
        }

        ByteArrayOutputStream scb=new ByteArrayOutputStream(byte5.length()/2);
        for(int i=0;i<byte5.length();i+=2) {
            scb.write((hexString.indexOf(byte5.charAt(i)) << 4 | hexString.indexOf(byte5.charAt(i + 1))));
        }

        return new String(scb.toByteArray());
    }

}



// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0