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

class cl_12 
{
	   float var_15 = (1.55212e-14F);
	   byte var_39 = (byte)(94);
	final   double [] var_45 = {(-6.88375e+41D),(-1.85307e+157D)};
	   long var_48 = (-3660548890658865655L);
	   int var_51 = (-1326943148);
	   short var_54 = (short)(26835);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(35);
		CrcCheck.ToByteArray(this.var_15,b,"var_15");
		CrcCheck.ToByteArray(this.var_39,b,"var_39");
		for(int a0=0;a0<2;++a0){
			CrcCheck.ToByteArray(this.var_45[a0],b,"var_45" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_48,b,"var_48");
		CrcCheck.ToByteArray(this.var_51,b,"var_51");
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
