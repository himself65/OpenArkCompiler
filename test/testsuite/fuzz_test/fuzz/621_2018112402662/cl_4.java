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

//SEED=1902118766

//import java.util.zip.CRC32;

class cl_4 
{
	   float var_18 = (-6.27166e+20F);
	   byte var_43 = (byte)(-34);
	   int var_54 = (-2084762166);
	   boolean var_74 = true;
	   double var_89 = (-6.48156e-10D);
	final   boolean [][][] var_96 = {{{false,true,true,true,true,false},{false,false,false,false,false,false},{true,false,false,false,false,false},{true,false,false,true,true,true},{true,false,true,true,true,false},{false,false,true,false,true,true},{false,false,false,false,false,false}},{{true,false,true,true,false,false},{false,false,true,false,true,true},{true,false,false,true,false,false},{true,false,true,false,false,false},{true,false,false,true,false,false},{false,true,true,true,false,false},{true,false,true,true,false,true}},{{true,false,false,false,true,true},{false,true,false,true,true,true},{false,false,true,false,true,true},{true,false,false,true,false,false},{true,false,true,false,false,true},{false,true,true,true,false,false},{false,true,true,true,true,true}},{{true,false,true,false,false,true},{false,true,false,true,false,false},{true,true,true,false,true,true},{false,false,false,false,false,false},{false,true,true,true,false,false},{true,true,true,false,false,false},{false,true,true,true,false,true}}};
	   double [][] var_101 = {{(-1.34918e-254D),(-3.86913e+110D),(-4.41259e+221D),(1.13389e+41D)},{(-2.74423e-47D),(-7.43346e+195D),(-9.02865e-201D),(1.64736e+188D)},{(-2.59179e+105D),(1.43757e-126D),(-1.70909e-266D),(-5.2853e-30D)},{(3.27181e+164D),(6.32469e+42D),(-1.62527e+123D),(9.01616e-78D)}};
	   double [] var_240 = {(1.83708e-26D)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(322);
		CrcCheck.ToByteArray(this.var_18,b,"var_18");
		CrcCheck.ToByteArray(this.var_43,b,"var_43");
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		CrcCheck.ToByteArray(this.var_74,b,"var_74");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_96[a0][a1][a2],b,"var_96" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_101[a0][a1],b,"var_101" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_240[a0],b,"var_240" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
