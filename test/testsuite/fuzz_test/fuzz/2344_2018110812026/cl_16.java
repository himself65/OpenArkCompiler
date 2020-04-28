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

//SEED=1360628436

import java.util.zip.CRC32;

class cl_16 
{
	   double var_33 = (4.82747e-253D);
	   byte var_35 = (byte)(120);
	   int [][] var_52 = {{(133955242),(1642502907),(1375724831),(-1898416015)},{(564634648),(-507609140),(-393080150),(-703549017)},{(-797298299),(-417529472),(802583456),(871538127)},{(-1323007981),(1613806131),(-1694709398),(1100081521)}};
	   boolean var_54 = false;
	final   short var_57 = (short)(-31317);
	   int var_67 = (-1235117507);
	   float [][] var_100 = {{(-7.38113e+25F),(-10.806F),(2.83154e-20F),(2.32975e+15F),(-4.27469e-26F)},{(6.23293e+30F),(1.24635e+34F),(-1.66331F),(3.96657e-10F),(-1.57215e-13F)},{(4.49792e-12F),(-4.53022e+06F),(8.07742e+19F),(1.02638e+09F),(9.88245e+21F)},{(-3.19923e+09F),(-5.74501e-37F),(-0.00145594F),(2.77709e-31F),(3.70232e-35F)},{(0.586482F),(2.2871e-16F),(6.17146e+07F),(-2.88526e+29F),(-3.02558e-35F)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(180);
		CrcCheck.ToByteArray(this.var_33,b,"var_33");
		CrcCheck.ToByteArray(this.var_35,b,"var_35");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_52[a0][a1],b,"var_52" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		CrcCheck.ToByteArray(this.var_57,b,"var_57");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_100[a0][a1],b,"var_100" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
