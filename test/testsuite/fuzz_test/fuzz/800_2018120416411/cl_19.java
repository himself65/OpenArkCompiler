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

//SEED=1217165572

//import java.util.zip.CRC32;

class cl_19 
{
	   byte var_21 = (byte)(-64);
	   float var_63 = (-6.11338F);
	   int var_71 = (-1715378809);
	   double [] var_82 = {(-1.21331e-51D),(6.08906e-271D),(-1.91733e+250D),(-2.26286e+169D),(-3.37683e-255D)};
	   double var_97 = (-4.51619e-263D);
	   boolean [] var_233 = {true,true,false};
	final   boolean var_306 = true;
	   long var_322 = (6538640244752615634L);
	   short var_348 = (short)(-10444);
	   byte [][][] var_369 = {{{(byte)(-52)},{(byte)(-95)},{(byte)(-63)},{(byte)(34)},{(byte)(-7)},{(byte)(-62)},{(byte)(38)}},{{(byte)(-126)},{(byte)(41)},{(byte)(122)},{(byte)(-105)},{(byte)(-50)},{(byte)(76)},{(byte)(-5)}},{{(byte)(-126)},{(byte)(91)},{(byte)(43)},{(byte)(-29)},{(byte)(-15)},{(byte)(81)},{(byte)(-123)}},{{(byte)(83)},{(byte)(-86)},{(byte)(-116)},{(byte)(-71)},{(byte)(56)},{(byte)(108)},{(byte)(-74)}},{{(byte)(49)},{(byte)(-16)},{(byte)(-78)},{(byte)(68)},{(byte)(4)},{(byte)(55)},{(byte)(31)}},{{(byte)(-114)},{(byte)(-11)},{(byte)(-62)},{(byte)(-18)},{(byte)(108)},{(byte)(89)},{(byte)(-22)}},{{(byte)(57)},{(byte)(123)},{(byte)(57)},{(byte)(-8)},{(byte)(-42)},{(byte)(-87)},{(byte)(22)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(120);
		CrcCheck.ToByteArray(this.var_21,b,"var_21");
		CrcCheck.ToByteArray(this.var_63,b,"var_63");
		CrcCheck.ToByteArray(this.var_71,b,"var_71");
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_82[a0],b,"var_82" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_97,b,"var_97");
		for(int a0=0;a0<3;++a0){
			CrcCheck.ToByteArray(this.var_233[a0],b,"var_233" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_306,b,"var_306");
		CrcCheck.ToByteArray(this.var_322,b,"var_322");
		CrcCheck.ToByteArray(this.var_348,b,"var_348");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_369[a0][a1][a2],b,"var_369" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
