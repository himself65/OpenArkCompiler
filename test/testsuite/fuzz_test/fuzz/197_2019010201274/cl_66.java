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

//SEED=2025510452

//import java.util.zip.CRC32;

class cl_66 
{
	final   int var_55 = (-1929671413);
	   byte var_81 = (byte)(-69);
	   long [] var_89 = {(-5492793535263746725L),(-3974566652130550779L),(1325887239593102764L),(-565464146090203449L),(-838819357738307655L),(1840211928353376327L),(-2739011310632093486L)};
	   long var_94 = (5012846206849273930L);
	   float var_102 = (9.22988e-37F);
	   byte [][][] var_107 = {{{(byte)(53),(byte)(19),(byte)(-94),(byte)(77)},{(byte)(103),(byte)(-3),(byte)(-119),(byte)(72)},{(byte)(-116),(byte)(33),(byte)(17),(byte)(-113)},{(byte)(54),(byte)(-114),(byte)(43),(byte)(-9)}}};
	   boolean var_116 = true;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(90);
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_89[a0],b,"var_89" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_94,b,"var_94");
		CrcCheck.ToByteArray(this.var_102,b,"var_102");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_107[a0][a1][a2],b,"var_107" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_116,b,"var_116");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
