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

//SEED=4207841504

import java.util.zip.CRC32;

class cl_30 
{
	   byte var_51 = (byte)(-60);
	   float var_53 = (-0.000238842F);
	   int var_60 = (-1180599096);
	   short var_67 = (short)(-24652);
	final   long [] var_79 = {(6923186734960061747L),(8998571473782451298L),(481779532784134326L),(2945776523141392048L),(958507896735344602L),(3002565063472025662L)};
	final   double var_98 = (2.75558e+130D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(67);
		CrcCheck.ToByteArray(this.var_51,b,"var_51");
		CrcCheck.ToByteArray(this.var_53,b,"var_53");
		CrcCheck.ToByteArray(this.var_60,b,"var_60");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		for(int a0=0;a0<6;++a0){
			CrcCheck.ToByteArray(this.var_79[a0],b,"var_79" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
