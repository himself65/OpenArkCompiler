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

//SEED=2759125148

//import java.util.zip.CRC32;

class cl_95 
{
	   double [][] var_85 = {{(0.196765D),(2.32914e-207D),(1.72286e-177D),(-2.2817e-283D),(-1.3731e-45D)}};
	final   double var_92 = (-2.49553e-159D);
	   double var_103 = (7.39416e+136D);
	   short var_108 = (short)(-10041);
	   boolean var_136 = false;
	   int var_150 = (-1426997677);
	final   byte var_160 = (byte)(-28);
	   long var_206 = (-6559537351150230800L);
	   float var_297 = (-1.93994e-23F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(76);
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_85[a0][a1],b,"var_85" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_92,b,"var_92");
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		CrcCheck.ToByteArray(this.var_136,b,"var_136");
		CrcCheck.ToByteArray(this.var_150,b,"var_150");
		CrcCheck.ToByteArray(this.var_160,b,"var_160");
		CrcCheck.ToByteArray(this.var_206,b,"var_206");
		CrcCheck.ToByteArray(this.var_297,b,"var_297");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
