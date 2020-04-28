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

//SEED=2981708336

//import java.util.zip.CRC32;

class cl_91 
{
	final   int var_75 = (61451722);
	   int var_109 = (467108756);
	   double var_111 = (-2.3343e+188D);
	   boolean var_112 = true;
	   cl_111 var_139 = new cl_111();
	   cl_10 var_146 = new cl_10();
	   cl_14 var_147 = new cl_14();
	   short var_308 = (short)(16803);
	   cl_80 var_375 = new cl_80();
	   cl_32 var_503 = new cl_32();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(59);
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
		CrcCheck.ToByteArray(this.var_111,b,"var_111");
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		CrcCheck.ToByteArray(this.var_139.GetChecksum(),b,"var_139.GetChecksum()");
		CrcCheck.ToByteArray(this.var_146.GetChecksum(),b,"var_146.GetChecksum()");
		CrcCheck.ToByteArray(this.var_147.GetChecksum(),b,"var_147.GetChecksum()");
		CrcCheck.ToByteArray(this.var_308,b,"var_308");
		CrcCheck.ToByteArray(this.var_375.GetChecksum(),b,"var_375.GetChecksum()");
		CrcCheck.ToByteArray(this.var_503.GetChecksum(),b,"var_503.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
