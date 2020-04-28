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

//SEED=1786270304

import java.util.zip.CRC32;

class cl_3 
{
	   int var_19 = (1639914758);
	   float var_21 = (1.24873e-12F);
	   boolean var_59 = true;
	   long var_66 = (-176521258812002281L);
	final   short var_67 = (short)(19414);
	   byte var_83 = (byte)(-25);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(20);
		CrcCheck.ToByteArray(this.var_19,b,"var_19");
		CrcCheck.ToByteArray(this.var_21,b,"var_21");
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_66,b,"var_66");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
