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

//SEED=205973598

//import java.util.zip.CRC32;

class cl_37 
{
	   float [][] var_51 = {{(-1.5111e-21F),(-4.69207e+36F),(2.04196e-12F),(-9.51574e-14F),(4.05267e-20F),(5.77881e+22F),(-9.13087e+19F)}};
	   boolean var_75 = false;
	   double var_80 = (-7.99438e-10D);
	   int [][][] var_91 = {{{(551547644)},{(-1044396287)},{(-935941465)}},{{(928213280)},{(1619975602)},{(341022479)}},{{(-1973984621)},{(1756070411)},{(1300155112)}},{{(1921850736)},{(845859766)},{(1027146520)}},{{(2141568905)},{(-1684718438)},{(-217648498)}}};
	   int var_125 = (-470597138);
	   short var_128 = (short)(23616);
	   long var_141 = (-2101988099306245011L);
	final   byte var_173 = (byte)(-49);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(112);
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_51[a0][a1],b,"var_51" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_80,b,"var_80");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_91[a0][a1][a2],b,"var_91" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_125,b,"var_125");
		CrcCheck.ToByteArray(this.var_128,b,"var_128");
		CrcCheck.ToByteArray(this.var_141,b,"var_141");
		CrcCheck.ToByteArray(this.var_173,b,"var_173");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
