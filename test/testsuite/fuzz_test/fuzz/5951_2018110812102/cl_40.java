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

//SEED=1786270304

import java.util.zip.CRC32;

class cl_40 
{
	   short var_49 = (short)(29150);
	   long var_54 = (6842624259198515784L);
	   boolean var_55 = true;
	   double var_97 = (-1.42623e-78D);
	final   int var_106 = (578961800);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(23);
		CrcCheck.ToByteArray(this.var_49,b,"var_49");
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_97,b,"var_97");
		CrcCheck.ToByteArray(this.var_106,b,"var_106");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
