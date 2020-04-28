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

//SEED=1157171100

//import java.util.zip.CRC32;

class cl_106 
{
	   int var_67 = (-2066283302);
	   cl_46 var_77 = new cl_46();
	   cl_39 var_97 = new cl_39();
	   cl_24 var_221 = new cl_24();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(28);
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CrcCheck.ToByteArray(this.var_77.GetChecksum(),b,"var_77.GetChecksum()");
		CrcCheck.ToByteArray(this.var_97.GetChecksum(),b,"var_97.GetChecksum()");
		CrcCheck.ToByteArray(this.var_221.GetChecksum(),b,"var_221.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
