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

//SEED=1438312272

import java.util.zip.CRC32;

class cl_8 
{
	   float var_17 = (2.28423e+37F);
	   boolean [][] var_20 = {{false,true,true},{false,false,false},{false,true,false},{false,false,true},{true,true,false},{true,false,true}};
	   boolean var_41 = true;
	final   long var_65 = (-2049594020404315578L);
	   int var_83 = (-1676829792);
	   short var_91 = (short)(4532);
	   double [][] var_99 = {{(-5.60182e-25D)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(45);
		CrcCheck.ToByteArray(this.var_17,b,"var_17");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_20[a0][a1],b,"var_20" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_41,b,"var_41");
		CrcCheck.ToByteArray(this.var_65,b,"var_65");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<1;++a1){
			CrcCheck.ToByteArray(this.var_99[a0][a1],b,"var_99" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
