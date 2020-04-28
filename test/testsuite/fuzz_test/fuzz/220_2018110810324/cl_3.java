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

//SEED=364962514

import java.util.zip.CRC32;

class cl_3 
{
	   int var_20 = (-1377176647);
	   long var_36 = (-5666264291173094202L);
	   double [] var_43 = {(9.66689e-191D),(8.77264e+124D)};
	final   double var_89 = (-3.37913e-168D);
	   boolean var_101 = false;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(37);
		CrcCheck.ToByteArray(this.var_20,b,"var_20");
		CrcCheck.ToByteArray(this.var_36,b,"var_36");
		for(int a0=0;a0<2;++a0){
			CrcCheck.ToByteArray(this.var_43[a0],b,"var_43" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_101,b,"var_101");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
