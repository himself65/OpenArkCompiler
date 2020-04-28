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

//SEED=432072564

//import java.util.zip.CRC32;

class cl_59 
{
	   float var_71 = (-8.37892e-13F);
	   long var_109 = (1024365099964343711L);
	final   short var_115 = (short)(6275);
	   boolean var_126 = true;
	   int var_129 = (524819925);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(19);
		CrcCheck.ToByteArray(this.var_71,b,"var_71");
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
		CrcCheck.ToByteArray(this.var_115,b,"var_115");
		CrcCheck.ToByteArray(this.var_126,b,"var_126");
		CrcCheck.ToByteArray(this.var_129,b,"var_129");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
