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

//SEED=764790408

//import java.util.zip.CRC32;

class cl_37 
{
	final   boolean var_37 = false;
	   int var_62 = (2091024729);
	   float var_77 = (-7932.42F);
	   boolean var_97 = false;
	   short var_104 = (short)(32313);
	   long var_153 = (-7957439555808437802L);
	   byte var_174 = (byte)(119);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(21);
		CrcCheck.ToByteArray(this.var_37,b,"var_37");
		CrcCheck.ToByteArray(this.var_62,b,"var_62");
		CrcCheck.ToByteArray(this.var_77,b,"var_77");
		CrcCheck.ToByteArray(this.var_97,b,"var_97");
		CrcCheck.ToByteArray(this.var_104,b,"var_104");
		CrcCheck.ToByteArray(this.var_153,b,"var_153");
		CrcCheck.ToByteArray(this.var_174,b,"var_174");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
