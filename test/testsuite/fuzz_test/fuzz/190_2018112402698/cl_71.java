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

//SEED=189333068

//import java.util.zip.CRC32;

class cl_71 
{
	   short var_51 = (short)(-8172);
	   byte var_52 = (byte)(111);
	   double var_56 = (-2.52811e+73D);
	   double [][][] var_57 = {{{(7.44062e+122D),(-1.48833e+98D),(-4.68551e+226D)}},{{(-3.08708e+68D),(-3.8553e+214D),(-5.47338e+24D)}}};
	   int var_67 = (-1407847278);
	   long var_207 = (-4216181795972826303L);
	   float var_212 = (-1.38623e-25F);
	   boolean [] var_229 = {true};
	   int [][] var_266 = {{(1303579896),(-821997073)},{(861969958),(-1395133466)}};
	   long [] var_336 = {(5941580355835384699L),(-7660351526588789941L),(-174322992065073533L),(-7618971873478779797L),(-6234703578627310592L)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(132);
		CrcCheck.ToByteArray(this.var_51,b,"var_51");
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<3;++a2){
			CrcCheck.ToByteArray(this.var_57[a0][a1][a2],b,"var_57" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CrcCheck.ToByteArray(this.var_207,b,"var_207");
		CrcCheck.ToByteArray(this.var_212,b,"var_212");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_229[a0],b,"var_229" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_266[a0][a1],b,"var_266" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_336[a0],b,"var_336" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
