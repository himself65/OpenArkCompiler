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

//SEED=545827268

import java.util.zip.CRC32;

class cl_13 
{
	   boolean var_44 = true;
	   long var_63 = (-1819499959988807919L);
	   byte var_69 = (byte)(92);
	   int var_80 = (716149719);
	   short var_103 = (short)(12222);
	   float var_111 = (-1.80655e+08F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(20);
		CrcCheck.ToByteArray(this.var_44,b,"var_44");
		CrcCheck.ToByteArray(this.var_63,b,"var_63");
		CrcCheck.ToByteArray(this.var_69,b,"var_69");
		CrcCheck.ToByteArray(this.var_80,b,"var_80");
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_111,b,"var_111");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
