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

//SEED=764790408

//import java.util.zip.CRC32;

class cl_144 
{
	   int var_98 = (-100250005);
	   boolean var_99 = false;
	   float var_107 = (-1.42034e+23F);
	final   short var_129 = (short)(19067);
	   double var_219 = (-1.44871e-84D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(19);
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		CrcCheck.ToByteArray(this.var_99,b,"var_99");
		CrcCheck.ToByteArray(this.var_107,b,"var_107");
		CrcCheck.ToByteArray(this.var_129,b,"var_129");
		CrcCheck.ToByteArray(this.var_219,b,"var_219");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
