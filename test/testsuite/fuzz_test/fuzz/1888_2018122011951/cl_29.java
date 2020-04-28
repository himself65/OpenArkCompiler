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

//SEED=1925474860
//import java.util.zip.CRC32;
class cl_29 
{
	   double var_82 = (-4.21015e-135D);
	   short var_98 = (short)(-23460);
	   int var_104 = (1712617002);
	   long var_116 = (-5815869228292543848L);
	   byte var_136 = (byte)(-35);
/*********************************/
	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(23);
		CrcCheck.ToByteArray(this.var_82,b,"var_82");
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		CrcCheck.ToByteArray(this.var_104,b,"var_104");
		CrcCheck.ToByteArray(this.var_116,b,"var_116");
		CrcCheck.ToByteArray(this.var_136,b,"var_136");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
