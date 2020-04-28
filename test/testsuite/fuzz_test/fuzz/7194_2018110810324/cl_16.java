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

class cl_16 
{
	   int var_31 = (-1255233882);
	   byte var_45 = (byte)(98);
	   int [] var_57 = {(-130302125),(451555484),(-868134140),(2086751195),(639120306),(-1475914604),(1641071936)};
	   double var_70 = (1.61675e+275D);
	   long var_71 = (7871318983280659914L);
	   short var_94 = (short)(22707);
	   float var_106 = (-3.17688e-13F);
	   double [][] var_165 = {{(2.04494e+158D),(4.20659e-140D)}};
	final   double [][] var_204 = {{(-4.97274e+261D),(-1.53828e-209D),(3.95222e-118D),(7.84205e-221D)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(103);
		CrcCheck.ToByteArray(this.var_31,b,"var_31");
		CrcCheck.ToByteArray(this.var_45,b,"var_45");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_57[a0],b,"var_57" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_71,b,"var_71");
		CrcCheck.ToByteArray(this.var_94,b,"var_94");
		CrcCheck.ToByteArray(this.var_106,b,"var_106");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_165[a0][a1],b,"var_165" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_204[a0][a1],b,"var_204" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
