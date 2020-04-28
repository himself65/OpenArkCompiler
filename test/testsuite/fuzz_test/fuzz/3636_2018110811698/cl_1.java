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

class cl_1 
{
	   float var_14 = (-4.38136e-31F);
	   byte var_34 = (byte)(106);
	   long var_36 = (1531311117969934958L);
	   boolean var_40 = true;
	   double var_43 = (-1.96711e-88D);
	   int var_52 = (-1476327195);
	   short var_89 = (short)(2467);
	final   boolean [][][] var_289 = {{{true,true,true,true}},{{true,true,false,true}},{{true,true,false,false}},{{true,false,false,false}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(44);
		CrcCheck.ToByteArray(this.var_14,b,"var_14");
		CrcCheck.ToByteArray(this.var_34,b,"var_34");
		CrcCheck.ToByteArray(this.var_36,b,"var_36");
		CrcCheck.ToByteArray(this.var_40,b,"var_40");
		CrcCheck.ToByteArray(this.var_43,b,"var_43");
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_289[a0][a1][a2],b,"var_289" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
