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

//SEED=764790408

//import java.util.zip.CRC32;

class cl_96 
{
	   int var_84 = (1525597661);
	   float [][][] var_94 = {{{(-1.02368e-14F),(1.91413e-15F)},{(0.000143955F),(5.40125e+14F)},{(340310F),(-2.26866e+30F)},{(6.50668e+35F),(265611F)},{(-1.6758e+21F),(6.06965e-15F)},{(-457999F),(-2.91196e+06F)},{(-2.40429e-35F),(-1.85702e+08F)}},{{(7.1143e-28F),(0.0234171F)},{(-2.49526e+29F),(-1.3839e-30F)},{(1.58576e-10F),(-8.30638e-37F)},{(1.29498e+20F),(-4.34371e-31F)},{(8.48755e+35F),(5.91403e-17F)},{(1.204e-22F),(-3.68927e+17F)},{(-4.68516e-06F),(2.2018e+33F)}}};
	   double var_100 = (8.78332e-207D);
	final   short var_103 = (short)(-19398);
	final   byte var_117 = (byte)(-98);
	   boolean var_168 = false;
	   float [][] var_279 = {{(1.05203e-12F),(-7.34983F),(-3.91121e-16F),(4.02461F),(4.28812e-20F),(0.116055F),(2.3627e-10F)},{(1.96275e+37F),(8.38502e-22F),(0.00346377F),(-2.1909e-26F),(-4.26688e-21F),(0.00474246F),(-1.3438e+35F)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(184);
		CrcCheck.ToByteArray(this.var_84,b,"var_84");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_94[a0][a1][a2],b,"var_94" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_100,b,"var_100");
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_117,b,"var_117");
		CrcCheck.ToByteArray(this.var_168,b,"var_168");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_279[a0][a1],b,"var_279" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
