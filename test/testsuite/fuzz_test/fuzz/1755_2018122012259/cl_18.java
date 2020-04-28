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

class cl_18 
{
	   double var_45 = (6.55696e-178D);
	   int var_49 = (588681790);
	   long var_96 = (7077514783226818085L);
	   boolean [] var_103 = {true};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(21);
		CrcCheck.ToByteArray(this.var_45,b,"var_45");
		CrcCheck.ToByteArray(this.var_49,b,"var_49");
		CrcCheck.ToByteArray(this.var_96,b,"var_96");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_103[a0],b,"var_103" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
