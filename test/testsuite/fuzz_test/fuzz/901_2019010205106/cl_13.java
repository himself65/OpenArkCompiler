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

class cl_13 
{
	   int var_51 = (521886791);
	   short var_67 = (short)(-24192);
	   float var_106 = (-5.33762e+13F);
	   int [] var_144 = {(255938857),(571133095),(2055296712),(-874156808),(-922067068),(-991268056),(2041932737)};
	   double var_165 = (-4.83364e-221D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(46);
		CrcCheck.ToByteArray(this.var_51,b,"var_51");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CrcCheck.ToByteArray(this.var_106,b,"var_106");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_144[a0],b,"var_144" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_165,b,"var_165");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
