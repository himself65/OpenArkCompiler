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

//SEED=3943380884

//import java.util.zip.CRC32;

class cl_31 
{
	   int var_65 = (541337861);
	final   long [] var_75 = {(4936014631523461438L)};
	   short var_107 = (short)(-14235);
	   float var_149 = (5.04368e-34F);
	   double var_168 = (9.27178e-303D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(26);
		CrcCheck.ToByteArray(this.var_65,b,"var_65");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_75[a0],b,"var_75" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_107,b,"var_107");
		CrcCheck.ToByteArray(this.var_149,b,"var_149");
		CrcCheck.ToByteArray(this.var_168,b,"var_168");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
