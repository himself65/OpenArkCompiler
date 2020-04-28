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

class cl_158 
{
	   int var_103 = (243717256);
	   short var_105 = (short)(-30161);
	   byte var_107 = (byte)(81);
	   float var_131 = (2.97516e+06F);
	   double [][][] var_148 = {{{(-1.2679e-200D),(-6.83846e+272D),(-1.23167e-233D),(-6.25959e+252D)},{(5.07106e-263D),(-7.85457e+90D),(6.79309e+170D),(-4.3805e+235D)},{(1.46404e-214D),(3.16763e-282D),(-1.66993e+37D),(4.56142e+54D)},{(-2.84781e+51D),(-9.35056e+121D),(-6.11917e-281D),(-7.81812e+305D)}},{{(-3.85101e-115D),(-9.53541e+113D),(-2.76441e-121D),(-3.0427e+103D)},{(-6.10027e-305D),(3.49164e+288D),(4.72711e+06D),(2.31844e+261D)},{(4.11167e-296D),(1.25148e+205D),(1.49732e-295D),(-6.799e+142D)},{(1.22351e-72D),(2.56804e-263D),(-3.22562e+260D),(6.60299e+40D)}},{{(4.91434e-275D),(-1.51786e-168D),(1.22247e+266D),(-6.00836e+294D)},{(241143D),(3.37135e-15D),(-5.12627e+213D),(-1.26898e+96D)},{(-1.15823e-181D),(-1.19793e-230D),(5.47505e+64D),(3.05376e-158D)},{(-3.34678e-223D),(6.16191e-179D),(1.28315e-110D),(-8.04407e-84D)}}};
	   long var_160 = (-9009921264744552029L);
	   double var_192 = (-5.59549e-244D);
	   boolean var_533 = true;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(412);
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		CrcCheck.ToByteArray(this.var_107,b,"var_107");
		CrcCheck.ToByteArray(this.var_131,b,"var_131");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_148[a0][a1][a2],b,"var_148" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_160,b,"var_160");
		CrcCheck.ToByteArray(this.var_192,b,"var_192");
		CrcCheck.ToByteArray(this.var_533,b,"var_533");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
