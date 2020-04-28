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

//SEED=3331378896

//import java.util.zip.CRC32;

class cl_66 
{
	   boolean var_70 = true;
	   short var_77 = (short)(2672);
	   byte var_83 = (byte)(-83);
	   int var_84 = (724027069);
	   float [][][] var_94 = {{{(-2.57321e-23F),(8.99001e-14F),(-5.6575e+06F),(-0.000189227F),(-1.27617e+29F)}},{{(2.92318e-06F),(-7.6509e+31F),(4.87687e+12F),(3.30772e-09F),(4.40802e-09F)}},{{(-1049.37F),(4.24468e+08F),(-1.62298e-12F),(6.52782e+27F),(0.000153332F)}},{{(1.9766e+34F),(5.83881e-38F),(3.98911e-17F),(7.11056e+20F),(1.06594e-26F)}},{{(-0.00685871F),(2.02767e+28F),(5.9937e+18F),(-1.78378e-36F),(3.03863F)}}};
	   double var_105 = (8.40271e-303D);
	   boolean [] var_109 = {false,false,false,true};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(120);
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_77,b,"var_77");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CrcCheck.ToByteArray(this.var_84,b,"var_84");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_94[a0][a1][a2],b,"var_94" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_109[a0],b,"var_109" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
