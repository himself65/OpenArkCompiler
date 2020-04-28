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

//SEED=59004558

//import java.util.zip.CRC32;

class cl_21 
{
	   boolean var_32 = false;
	final   byte var_57 = (byte)(-95);
	   long var_70 = (-3180082731109632451L);
	   double var_75 = (3.49285e-108D);
	   short var_78 = (short)(28059);
	   float var_105 = (-8.97857e+11F);
	   int var_116 = (1790416014);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(28);
		CrcCheck.ToByteArray(this.var_32,b,"var_32");
		CrcCheck.ToByteArray(this.var_57,b,"var_57");
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_78,b,"var_78");
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		CrcCheck.ToByteArray(this.var_116,b,"var_116");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
