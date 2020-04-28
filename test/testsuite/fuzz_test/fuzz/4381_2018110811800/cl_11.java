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

//SEED=4093507686

import java.util.zip.CRC32;

class cl_11 
{
	   double var_40 = (-2.90194e-293D);
	final   int var_48 = (1150143658);
	   short var_49 = (short)(5132);
	   float var_54 = (-3.26437e+07F);
	   byte var_56 = (byte)(122);
	final   double [][] var_126 = {{(2.53572e-36D),(-2.49758e+101D)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(35);
		CrcCheck.ToByteArray(this.var_40,b,"var_40");
		CrcCheck.ToByteArray(this.var_48,b,"var_48");
		CrcCheck.ToByteArray(this.var_49,b,"var_49");
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_126[a0][a1],b,"var_126" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
