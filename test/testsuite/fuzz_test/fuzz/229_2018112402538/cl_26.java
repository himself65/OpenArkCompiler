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

//SEED=59004558

//import java.util.zip.CRC32;

class cl_26 
{
	   boolean var_60 = false;
	final   int var_111 = (557036228);
	final   byte var_125 = (byte)(-34);
	final   double var_145 = (-2.04762e-112D);
	final   float var_299 = (2.11043e-16F);
	   short var_369 = (short)(8932);
	   double var_386 = (-3.96772e+52D);
	   boolean [][][] var_479 = {{{true,true,true},{true,false,true},{true,true,true},{true,true,true},{false,true,true}},{{false,true,false},{false,true,false},{true,true,true},{true,false,true},{true,true,false}},{{false,false,true},{true,true,false},{false,true,false},{true,true,true},{true,true,true}}};
	final   long var_500 = (-1209936412644610351L);
	   long var_569 = (9093867896489650249L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(89);
		CrcCheck.ToByteArray(this.var_60,b,"var_60");
		CrcCheck.ToByteArray(this.var_111,b,"var_111");
		CrcCheck.ToByteArray(this.var_125,b,"var_125");
		CrcCheck.ToByteArray(this.var_145,b,"var_145");
		CrcCheck.ToByteArray(this.var_299,b,"var_299");
		CrcCheck.ToByteArray(this.var_369,b,"var_369");
		CrcCheck.ToByteArray(this.var_386,b,"var_386");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<3;++a2){
			CrcCheck.ToByteArray(this.var_479[a0][a1][a2],b,"var_479" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_500,b,"var_500");
		CrcCheck.ToByteArray(this.var_569,b,"var_569");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
