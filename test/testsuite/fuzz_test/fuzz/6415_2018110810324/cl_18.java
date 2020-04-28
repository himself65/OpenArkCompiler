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

//SEED=598886972

import java.util.zip.CRC32;

class cl_18 
{
	   double [] var_18 = {(-5.09019e+117D),(-5.36295e-211D),(-4.97261e-103D),(7.90617e-32D),(-1.2683e+234D)};
	   byte var_44 = (byte)(-12);
	   double [][] var_69 = {{(1.68625e-39D),(-2.69203e+306D),(4.53354e-168D)},{(7.74072e-153D),(1.37772e-08D),(-7.33578e+225D)},{(-4.9939e+256D),(-5.98858e-98D),(-2.38134e+30D)},{(1.14277e-14D),(-5.95112e+57D),(1.0058e-270D)}};
	   int var_73 = (1382973826);
	   double var_74 = (-6.97378e-146D);
	   int [] var_81 = {(-101442534),(539025601),(563011210),(-1316568772),(996640050),(620269270)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(173);
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_18[a0],b,"var_18" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_44,b,"var_44");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_69[a0][a1],b,"var_69" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_73,b,"var_73");
		CrcCheck.ToByteArray(this.var_74,b,"var_74");
		for(int a0=0;a0<6;++a0){
			CrcCheck.ToByteArray(this.var_81[a0],b,"var_81" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
