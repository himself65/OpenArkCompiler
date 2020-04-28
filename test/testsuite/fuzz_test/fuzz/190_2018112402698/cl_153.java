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

//SEED=189333068

//import java.util.zip.CRC32;

class cl_153 
{
	   boolean [] var_131 = {true};
	   int var_252 = (2072773388);
	final   short var_309 = (short)(28292);
	   double var_325 = (-7.29179e+223D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(15);
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_131[a0],b,"var_131" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_252,b,"var_252");
		CrcCheck.ToByteArray(this.var_309,b,"var_309");
		CrcCheck.ToByteArray(this.var_325,b,"var_325");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
