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

//SEED=1739368500

import java.util.zip.CRC32;

class cl_2 
{
	   boolean [][] var_33 = {{false,false},{false,false}};
	   float var_47 = (-1.43591e+38F);
	final   long var_49 = (5016591342775283226L);
	   float [] var_50 = {(-2.18702e-05F),(5.93315e+18F),(-2.69541e+15F),(-1.33626e-13F)};
	   short var_56 = (short)(-9910);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(34);
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_33[a0][a1],b,"var_33" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_47,b,"var_47");
		CrcCheck.ToByteArray(this.var_49,b,"var_49");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_50[a0],b,"var_50" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
