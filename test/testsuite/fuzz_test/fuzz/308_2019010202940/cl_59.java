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

class cl_59 
{
	   float var_41 = (-7.80011e-33F);
	   int var_86 = (391708806);
	final   byte var_92 = (byte)(105);
	   double var_104 = (-1.00263e+62D);
	   long var_116 = (1040921520548197877L);
	   short var_179 = (short)(-8110);
	   int [] var_216 = {(975155499),(1928137490)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(35);
		CrcCheck.ToByteArray(this.var_41,b,"var_41");
		CrcCheck.ToByteArray(this.var_86,b,"var_86");
		CrcCheck.ToByteArray(this.var_92,b,"var_92");
		CrcCheck.ToByteArray(this.var_104,b,"var_104");
		CrcCheck.ToByteArray(this.var_116,b,"var_116");
		CrcCheck.ToByteArray(this.var_179,b,"var_179");
		for(int a0=0;a0<2;++a0){
			CrcCheck.ToByteArray(this.var_216[a0],b,"var_216" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
