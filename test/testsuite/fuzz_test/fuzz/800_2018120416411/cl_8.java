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

class cl_8 
{
	   boolean [] var_8 = {true,false,false};
	   byte var_25 = (byte)(18);
	   double var_51 = (1.70053e-214D);
	   long var_58 = (7614955000069593042L);
	   int var_66 = (1010800474);
	   double [] var_83 = {(-3.11422e+91D),(8.33068e-275D),(-1.97117e-245D),(-1.18694e-117D),(-1.73866e+29D)};
	   short var_154 = (short)(5237);
	final   float var_167 = (1.74644e-13F);
	   boolean var_205 = false;
	   byte [][][] var_373 = {{{(byte)(14)},{(byte)(-126)},{(byte)(-122)},{(byte)(38)},{(byte)(110)},{(byte)(-75)},{(byte)(-62)}},{{(byte)(-107)},{(byte)(-87)},{(byte)(-70)},{(byte)(37)},{(byte)(28)},{(byte)(41)},{(byte)(18)}},{{(byte)(38)},{(byte)(-51)},{(byte)(12)},{(byte)(-122)},{(byte)(33)},{(byte)(-35)},{(byte)(-74)}},{{(byte)(-126)},{(byte)(-36)},{(byte)(86)},{(byte)(-74)},{(byte)(-92)},{(byte)(-124)},{(byte)(97)}},{{(byte)(5)},{(byte)(-11)},{(byte)(76)},{(byte)(33)},{(byte)(92)},{(byte)(100)},{(byte)(-12)}},{{(byte)(27)},{(byte)(72)},{(byte)(29)},{(byte)(42)},{(byte)(-104)},{(byte)(85)},{(byte)(-68)}},{{(byte)(-43)},{(byte)(111)},{(byte)(69)},{(byte)(62)},{(byte)(-127)},{(byte)(12)},{(byte)(-125)}}};
	   float [][][] var_492 = {{{(-1.70835e-07F),(5.57924e-10F)},{(0.365472F),(2.46394e-19F)},{(1.59439e-09F),(-1.55721e-07F)}},{{(9.87914e-32F),(1.76375e+14F)},{(8.95587e+16F),(3.36658e-18F)},{(3.44704e-38F),(0.0897001F)}},{{(4.74537e+32F),(8.25314e-17F)},{(2.17595e+34F),(1.23368e-18F)},{(-2.84838e-22F),(1.38383e-19F)}},{{(6.96508e-11F),(2.09148e+18F)},{(-1.58819e-24F),(1.11326e+28F)},{(7.46797e-11F),(-2.58108e+32F)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(216);
		for(int a0=0;a0<3;++a0){
			CrcCheck.ToByteArray(this.var_8[a0],b,"var_8" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_25,b,"var_25");
		CrcCheck.ToByteArray(this.var_51,b,"var_51");
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		CrcCheck.ToByteArray(this.var_66,b,"var_66");
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_83[a0],b,"var_83" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_154,b,"var_154");
		CrcCheck.ToByteArray(this.var_167,b,"var_167");
		CrcCheck.ToByteArray(this.var_205,b,"var_205");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_373[a0][a1][a2],b,"var_373" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_492[a0][a1][a2],b,"var_492" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
