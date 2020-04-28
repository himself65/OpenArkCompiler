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

//SEED=562055798

//import java.util.zip.CRC32;

class cl_2 
{
	   double var_69 = (2.54755e+77D);
	   int var_84 = (-2144056568);
	final   short var_128 = (short)(-22137);
	   float var_164 = (-4.33295e+32F);
	final   float [] var_175 = {(-2.10171e+09F),(-8.40217e+21F),(35003.4F)};
	   boolean var_226 = false;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(31);
		CrcCheck.ToByteArray(this.var_69,b,"var_69");
		CrcCheck.ToByteArray(this.var_84,b,"var_84");
		CrcCheck.ToByteArray(this.var_128,b,"var_128");
		CrcCheck.ToByteArray(this.var_164,b,"var_164");
		for(int a0=0;a0<3;++a0){
			CrcCheck.ToByteArray(this.var_175[a0],b,"var_175" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_226,b,"var_226");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
