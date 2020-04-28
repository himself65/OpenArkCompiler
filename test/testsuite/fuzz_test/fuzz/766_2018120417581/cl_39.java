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

//SEED=2759125148

//import java.util.zip.CRC32;

class cl_39 
{
	   boolean var_48 = false;
	   float var_64 = (-0.000233833F);
	   double var_76 = (2.29126e-08D);
	   byte var_94 = (byte)(-33);
	   short var_98 = (short)(25616);
	   int var_129 = (-1709830416);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(20);
		CrcCheck.ToByteArray(this.var_48,b,"var_48");
		CrcCheck.ToByteArray(this.var_64,b,"var_64");
		CrcCheck.ToByteArray(this.var_76,b,"var_76");
		CrcCheck.ToByteArray(this.var_94,b,"var_94");
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		CrcCheck.ToByteArray(this.var_129,b,"var_129");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
