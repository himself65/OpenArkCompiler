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

//SEED=87166108

import java.util.zip.CRC32;

class cl_24 
{
	   int var_23 = (-431562253);
	   double var_37 = (4.81147e+83D);
	   short var_47 = (short)(11183);
	   byte var_51 = (byte)(-58);
	   boolean [][][] var_67 = {{{true,false,true,true,false},{false,false,false,false,true},{false,false,false,false,true},{false,false,false,false,true},{false,true,true,true,false},{true,true,true,false,false}},{{false,false,false,false,false},{false,false,true,false,false},{true,false,false,false,true},{false,false,true,true,true},{true,true,false,false,false},{true,false,false,true,true}},{{true,false,true,false,true},{false,false,true,true,false},{true,true,false,true,false},{true,true,true,false,false},{false,false,false,true,true},{true,true,true,false,false}},{{true,true,false,true,false},{false,false,true,true,false},{true,false,false,true,true},{true,true,false,true,true},{true,true,true,false,true},{true,false,false,true,true}},{{false,true,false,true,true},{true,false,true,false,false},{true,true,false,false,false},{true,true,true,true,false},{false,true,false,true,true},{true,false,true,false,true}},{{false,false,true,true,false},{false,true,true,false,true},{true,true,true,false,true},{false,false,false,false,false},{false,true,false,false,false},{true,false,true,false,true}},{{false,true,true,false,true},{true,true,false,true,false},{false,true,true,false,true},{false,true,false,false,true},{false,true,false,true,false},{false,false,false,false,true}}};
	   long var_118 = (3831756633247288239L);
	   float var_147 = (1.59305e-28F);
	   boolean var_166 = true;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(238);
		CrcCheck.ToByteArray(this.var_23,b,"var_23");
		CrcCheck.ToByteArray(this.var_37,b,"var_37");
		CrcCheck.ToByteArray(this.var_47,b,"var_47");
		CrcCheck.ToByteArray(this.var_51,b,"var_51");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_67[a0][a1][a2],b,"var_67" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_118,b,"var_118");
		CrcCheck.ToByteArray(this.var_147,b,"var_147");
		CrcCheck.ToByteArray(this.var_166,b,"var_166");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
