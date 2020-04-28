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

class cl_99 
{
	   float var_64 = (1.19187e-05F);
	   byte var_65 = (byte)(-42);
	   int var_66 = (861282567);
	final   boolean var_68 = false;
	   double var_137 = (-6.27547e+227D);
	   long var_157 = (1975717745457226122L);
	   boolean var_368 = true;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(27);
		CrcCheck.ToByteArray(this.var_64,b,"var_64");
		CrcCheck.ToByteArray(this.var_65,b,"var_65");
		CrcCheck.ToByteArray(this.var_66,b,"var_66");
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		CrcCheck.ToByteArray(this.var_137,b,"var_137");
		CrcCheck.ToByteArray(this.var_157,b,"var_157");
		CrcCheck.ToByteArray(this.var_368,b,"var_368");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
