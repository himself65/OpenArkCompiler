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

//SEED=3943380884

//import java.util.zip.CRC32;

class cl_73 
{
	   byte var_52 = (byte)(98);
	   long var_67 = (2457461225697564111L);
	final   float var_83 = (-1.10039e+32F);
	   int var_89 = (1398292841);
	   float var_147 = (-2.32574e+32F);
	   double var_171 = (5.8143e+86D);
	   short var_178 = (short)(-16491);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(31);
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_147,b,"var_147");
		CrcCheck.ToByteArray(this.var_171,b,"var_171");
		CrcCheck.ToByteArray(this.var_178,b,"var_178");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
