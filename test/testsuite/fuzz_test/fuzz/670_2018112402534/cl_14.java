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

//SEED=2981708336

//import java.util.zip.CRC32;

class cl_14 
{
	   float var_32 = (4.42068e-26F);
	   int var_83 = (-1242087480);
	   long var_108 = (4722309094298049512L);
	final   short var_148 = (short)(-22812);
	   float [][][] var_212 = {{{(5.57749e-08F),(6.02033e+12F),(-7.02594e-29F),(-8091.29F)}},{{(4.56317e-18F),(2.57889e+19F),(-3.84245e-22F),(-3.44177F)}},{{(4.25636e+18F),(-5.06088e-25F),(-3.03057e+22F),(-2.53232e-36F)}},{{(1.82334e-12F),(-1.8829e-28F),(-1.16318e+07F),(-8.04691e+30F)}},{{(7.11838e+25F),(2.64445e-36F),(6.68046e-19F),(1.32471e-20F)}},{{(-8.49903e-36F),(-3.83322e-32F),(-5.1681e+33F),(-2.66656e-17F)}}};
	   byte var_230 = (byte)(124);
	   double var_232 = (-4.10421e-67D);
	   int [] var_417 = {(-1195526260)};
	   boolean [][][] var_591 = {{{false,false,false,true,false,false,true},{false,false,false,true,true,true,true},{false,false,true,false,false,true,false},{false,true,true,false,true,false,false},{true,true,false,false,true,false,true},{true,true,true,false,true,false,false}}};
	   boolean var_621 = false;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(170);
		CrcCheck.ToByteArray(this.var_32,b,"var_32");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		CrcCheck.ToByteArray(this.var_148,b,"var_148");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_212[a0][a1][a2],b,"var_212" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_230,b,"var_230");
		CrcCheck.ToByteArray(this.var_232,b,"var_232");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_417[a0],b,"var_417" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_591[a0][a1][a2],b,"var_591" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_621,b,"var_621");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
