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

//SEED=3292958668

import java.util.zip.CRC32;

class cl_14 
{
	final   byte var_44 = (byte)(-47);
	   int var_48 = (1806542864);
	   short var_49 = (short)(-1991);
	   int [] var_58 = {(71153632),(-100256474),(-1636961078),(1211522729),(675021837),(1122974701),(2101825207)};
	   long var_74 = (5530017303593813387L);
	   double var_89 = (1.7851e-291D);
	   float var_143 = (5.72965e+16F);
	   float [][][] var_176 = {{{(1.04664e-18F),(-1.5446e+23F),(-0.003603F),(3.33612e+10F)}},{{(-1.41058e+29F),(-1.63989e-19F),(-2.69107e+12F),(-3.22266e+14F)}},{{(-4.10445e+15F),(-1.11868e-27F),(-0.0244894F),(-1.24037e+33F)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(103);
		CrcCheck.ToByteArray(this.var_44,b,"var_44");
		CrcCheck.ToByteArray(this.var_48,b,"var_48");
		CrcCheck.ToByteArray(this.var_49,b,"var_49");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_58[a0],b,"var_58" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_74,b,"var_74");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_143,b,"var_143");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_176[a0][a1][a2],b,"var_176" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
