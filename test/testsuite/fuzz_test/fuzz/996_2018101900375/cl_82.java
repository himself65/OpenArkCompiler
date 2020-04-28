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

class cl_82 
{
	   double var_55 = (-1.87945e+211D);
	   byte var_66 = (byte)(93);
	   short var_68 = (short)(1421);
	   float [] var_87 = {(1.27182e+09F),(-8.19522e-24F),(-9.59775e+27F),(-1.01147e-24F),(3.35866e-25F),(-4.77324e-18F),(-7.53344e+17F)};
	final   int var_293 = (431819329);
	   long var_323 = (1906199299932418711L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(51);
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_66,b,"var_66");
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_87[a0],b,"var_87" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_293,b,"var_293");
		CrcCheck.ToByteArray(this.var_323,b,"var_323");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
