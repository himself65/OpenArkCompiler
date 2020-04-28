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

class cl_21 
{
	   boolean var_24 = true;
	   double var_48 = (-1.23142e-304D);
	   int var_50 = (317723889);
	   boolean [][][] var_55 = {{{true,true,true,true,false,true}},{{false,false,true,false,true,true}},{{false,true,false,true,true,true}},{{false,true,true,true,false,true}}};
	   float var_59 = (-1.18099e-24F);
	   short var_100 = (short)(10488);
	final   boolean [][][] var_107 = {{{true,false},{false,false},{false,true}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(49);
		CrcCheck.ToByteArray(this.var_24,b,"var_24");
		CrcCheck.ToByteArray(this.var_48,b,"var_48");
		CrcCheck.ToByteArray(this.var_50,b,"var_50");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_55[a0][a1][a2],b,"var_55" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_100,b,"var_100");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_107[a0][a1][a2],b,"var_107" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
