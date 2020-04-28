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

//SEED=1808069720

//import java.util.zip.CRC32;

class cl_144 
{
	   boolean var_108 = false;
	   byte var_109 = (byte)(97);
	final   short var_113 = (short)(30379);
	   float var_118 = (2.60144e+06F);
	   double [][][] var_147 = {{{(-1.34578e-111D),(1.75205e-108D),(7.44932e+87D),(9.28859e-270D)},{(-2.21285e-66D),(-7.68923e-44D),(-4.5418e-190D),(-5.94974e+110D)},{(1.38598e+174D),(6.47491e+194D),(5.86957e+175D),(2.42507e-104D)},{(-1.27421e+47D),(3.11057e-110D),(1.50182e-146D),(-6.47538e-27D)}},{{(-3.32715e+20D),(2.17045e-203D),(-3.72283e-270D),(-4.35111e+232D)},{(2.05685e-75D),(-2.86426e+40D),(-1.61755e+34D),(-2.02114e+161D)},{(-4.13127e-263D),(2.99494e-133D),(6.9211e-152D),(-4.37345e+207D)},{(-1.25125e+125D),(-3.92635e+248D),(-4.34337e+206D),(-5.55423e+31D)}},{{(2.18193e-247D),(1.6154e+209D),(1.86219e+262D),(3.49428e-09D)},{(3.07512e-274D),(4.73869e+48D),(1.52174e-136D),(-2.2153e-233D)},{(3.02995e+77D),(5.30126e+14D),(-8.68377e+189D),(-2.81322e+54D)},{(-4.23975e+80D),(-2.94285e+257D),(-1.73376e-285D),(6.54655e-183D)}}};
	   long var_163 = (3880103892836116929L);
	   int var_180 = (-1947713262);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(404);
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
		CrcCheck.ToByteArray(this.var_113,b,"var_113");
		CrcCheck.ToByteArray(this.var_118,b,"var_118");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_147[a0][a1][a2],b,"var_147" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_163,b,"var_163");
		CrcCheck.ToByteArray(this.var_180,b,"var_180");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
