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

//SEED=1902118766

//import java.util.zip.CRC32;

class cl_7 
{
	   float var_11 = (2.18188e-30F);
	   byte var_13 = (byte)(28);
	   int var_29 = (-707691065);
	   boolean [][] var_63 = {{true,false,true},{false,true,true},{false,true,false},{true,false,true},{true,false,false},{false,true,false}};
	final   boolean [][][] var_97 = {{{false,false,false,true,true,true},{true,false,false,false,false,true},{true,true,false,true,true,true},{false,false,false,false,true,true},{true,true,true,false,true,true},{false,false,true,false,false,true},{false,false,false,true,false,true}},{{true,false,false,false,false,false},{false,false,false,true,false,true},{true,true,false,false,true,false},{true,true,true,true,true,true},{true,true,false,true,false,false},{false,false,true,true,true,true},{false,true,false,false,false,true}},{{false,true,true,true,false,true},{false,true,false,true,false,true},{true,true,false,false,true,false},{false,true,false,false,false,false},{false,true,true,false,false,true},{true,true,false,false,true,false},{false,false,true,false,false,true}},{{true,true,true,false,true,true},{true,true,true,true,true,true},{true,false,false,true,false,true},{true,false,false,false,false,false},{true,true,true,false,false,false},{true,true,true,false,false,true},{false,true,false,true,false,true}}};
	   long var_98 = (8898237846118272311L);
	   double var_217 = (-1.62707e+259D);
	final   short var_283 = (short)(16216);
	   boolean var_296 = false;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(214);
		CrcCheck.ToByteArray(this.var_11,b,"var_11");
		CrcCheck.ToByteArray(this.var_13,b,"var_13");
		CrcCheck.ToByteArray(this.var_29,b,"var_29");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_63[a0][a1],b,"var_63" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_97[a0][a1][a2],b,"var_97" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		CrcCheck.ToByteArray(this.var_217,b,"var_217");
		CrcCheck.ToByteArray(this.var_283,b,"var_283");
		CrcCheck.ToByteArray(this.var_296,b,"var_296");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
