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

//SEED=3930966242

import java.util.zip.CRC32;

class cl_49 
{
	   boolean var_34 = false;
	   boolean [][][] var_43 = {{{false,false,false,false,true,true}},{{true,true,false,false,false,false}},{{true,false,true,true,true,true}},{{false,true,false,true,false,true}}};
	   long var_52 = (-1691200957586921690L);
	   float [][][] var_111 = {{{(3.93341e+37F),(-8.07758e-26F)}},{{(-0.804115F),(-1.28436e+36F)}},{{(-4.92953e+30F),(-2.10421e+38F)}},{{(-2.80597e-18F),(5.45795e-26F)}},{{(8.13332e-14F),(-6.48659e-25F)}},{{(-254419F),(4.60475e+17F)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(81);
		CrcCheck.ToByteArray(this.var_34,b,"var_34");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_43[a0][a1][a2],b,"var_43" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_111[a0][a1][a2],b,"var_111" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
