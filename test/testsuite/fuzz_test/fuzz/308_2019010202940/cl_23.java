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

//SEED=2776528782

//import java.util.zip.CRC32;

class cl_23 
{
	   boolean var_21 = false;
	   short var_68 = (short)(-8908);
	   byte var_73 = (byte)(-13);
	   double var_145 = (7.55308e-157D);
	   int var_189 = (1069387508);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(16);
		CrcCheck.ToByteArray(this.var_21,b,"var_21");
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		CrcCheck.ToByteArray(this.var_73,b,"var_73");
		CrcCheck.ToByteArray(this.var_145,b,"var_145");
		CrcCheck.ToByteArray(this.var_189,b,"var_189");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
