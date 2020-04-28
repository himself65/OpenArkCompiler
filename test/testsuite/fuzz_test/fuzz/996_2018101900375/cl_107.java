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

//SEED=1808069720

//import java.util.zip.CRC32;

class cl_107 
{
	   int var_73 = (1005648514);
	   long var_111 = (8426351400870272196L);
	   short var_114 = (short)(-32716);
	   float var_382 = (6.35675e+18F);
	   double var_518 = (7.86978e-205D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(26);
		CrcCheck.ToByteArray(this.var_73,b,"var_73");
		CrcCheck.ToByteArray(this.var_111,b,"var_111");
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CrcCheck.ToByteArray(this.var_382,b,"var_382");
		CrcCheck.ToByteArray(this.var_518,b,"var_518");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
