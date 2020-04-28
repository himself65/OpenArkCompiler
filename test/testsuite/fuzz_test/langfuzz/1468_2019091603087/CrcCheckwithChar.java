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
 */

import java.lang.Double;

class CrcBuffer {
    int i = 0;
    static int buffer_counter = 0;
    byte[] buffer;

    CrcBuffer(int size) {
        this.buffer = new byte[size];
        buffer_counter++;
        //System.out.printf("Buffer-Counter=%d\tLength=%d\n", buffer_counter, size);
    }
}

public class CrcCheckwithChar {
    /**************************************************************/
    static boolean isVerbose = false;

    public static void ToByteArray(char args, CrcBuffer b) {
        b.buffer[b.i] = (byte) ((args >> 8) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) (args & 0xff);
        b.i++;
    }

    public static void ToByteArray(byte args, CrcBuffer b) {
        b.buffer[b.i] = args;
        b.i++;
    }

    public static void ToByteArray(short args, CrcBuffer b) {
        b.buffer[b.i] = (byte) ((args >> 8) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) (args & 0xff);
        b.i++;
    }

    public static void ToByteArray(int args, CrcBuffer b) {
        b.buffer[b.i] = (byte) ((args >> 24) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 16) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 8) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) (args & 0xff);
        b.i++;
    }

    public static void ToByteArray(long args, CrcBuffer b) {
        b.buffer[b.i] = (byte) ((args >> 56) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 48) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 40) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 32) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 24) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 16) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((args >> 8) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) (args & 0xff);
        b.i++;
    }

    public static void ToByteArray(boolean args, CrcBuffer b) {
        if (args) {
            b.buffer[b.i] = 0x01;
        } else {
            b.buffer[b.i] = 0x00;
        }
        b.i++;
    }

    public static void ToByteArray(float args, CrcBuffer b) {
        int iargs = Float.floatToIntBits(args);
        b.buffer[b.i] = (byte) ((iargs >> 24) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((iargs >> 16) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((iargs >> 8) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) (iargs & 0xff);
        b.i++;
    }

    public static void ToByteArray(double args, CrcBuffer b) {
        long largs = Double.doubleToLongBits(args);
        b.buffer[b.i] = (byte) ((largs >> 56) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((largs >> 48) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((largs >> 40) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((largs >> 32) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((largs >> 24) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((largs >> 16) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) ((largs >> 8) & 0xff);
        b.i++;
        b.buffer[b.i] = (byte) (largs & 0xff);
        b.i++;
    }
}
