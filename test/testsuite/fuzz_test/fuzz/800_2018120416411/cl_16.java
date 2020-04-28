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

class cl_16 
{
	   byte var_50 = (byte)(16);
	   int var_70 = (2060073326);
	   boolean var_95 = false;
	   float var_109 = (-4.04952e+31F);
	final   short var_161 = (short)(-16992);
	final   boolean [] var_176 = {true,true,true};
	   long var_365 = (-8163133472827597963L);
	final   byte [][][] var_372 = {{{(byte)(119)},{(byte)(-98)},{(byte)(17)},{(byte)(121)},{(byte)(-69)},{(byte)(-80)},{(byte)(48)}},{{(byte)(58)},{(byte)(118)},{(byte)(89)},{(byte)(80)},{(byte)(22)},{(byte)(107)},{(byte)(4)}},{{(byte)(-50)},{(byte)(0)},{(byte)(-103)},{(byte)(-66)},{(byte)(111)},{(byte)(17)},{(byte)(-108)}},{{(byte)(17)},{(byte)(118)},{(byte)(-101)},{(byte)(65)},{(byte)(-28)},{(byte)(-37)},{(byte)(-14)}},{{(byte)(30)},{(byte)(66)},{(byte)(6)},{(byte)(-14)},{(byte)(3)},{(byte)(-120)},{(byte)(84)}},{{(byte)(85)},{(byte)(-31)},{(byte)(41)},{(byte)(57)},{(byte)(120)},{(byte)(-108)},{(byte)(-91)}},{{(byte)(90)},{(byte)(101)},{(byte)(-101)},{(byte)(8)},{(byte)(-125)},{(byte)(48)},{(byte)(-56)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(72);
		CrcCheck.ToByteArray(this.var_50,b,"var_50");
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_95,b,"var_95");
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
		CrcCheck.ToByteArray(this.var_161,b,"var_161");
		for(int a0=0;a0<3;++a0){
			CrcCheck.ToByteArray(this.var_176[a0],b,"var_176" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_365,b,"var_365");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_372[a0][a1][a2],b,"var_372" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
