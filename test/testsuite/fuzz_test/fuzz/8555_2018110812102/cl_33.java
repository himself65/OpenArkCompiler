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

//SEED=4141941138

import java.util.zip.CRC32;

class cl_33 
{
	   int var_32 = (-1756798);
	   double [] var_52 = {(-2.10879e+294D),(2.83231e-198D),(-3.75486e-109D),(2.30186e-79D),(1.08115e+18D)};
	   float var_55 = (0.0236935F);
	   long var_59 = (-5304892024847098383L);
	   boolean [][] var_60 = {{true,true,true,false},{true,true,false,true},{true,true,true,true},{true,true,false,true},{false,false,true,false},{false,true,false,true},{true,true,true,true}};
	   byte var_61 = (byte)(37);
	   boolean var_62 = true;
	   short var_67 = (short)(-6058);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(88);
		CrcCheck.ToByteArray(this.var_32,b,"var_32");
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_52[a0],b,"var_52" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_60[a0][a1],b,"var_60" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_61,b,"var_61");
		CrcCheck.ToByteArray(this.var_62,b,"var_62");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
