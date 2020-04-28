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

//SEED=2981708336

//import java.util.zip.CRC32;

class cl_80 
{
	   int var_72 = (2096623525);
	   short var_77 = (short)(1951);
	   cl_32 var_98 = new cl_32();
	   cl_10 var_127 = new cl_10();
	   cl_14 var_376 = new cl_14();
	final   cl_32 [][][] var_381 = {{{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()}},{{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()}},{{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()}},{{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()},{new cl_32()}}};
	   boolean [][][] var_405 = {{{true,true,false,true,false,true,true},{true,true,true,true,true,false,true},{false,true,false,true,true,false,false},{true,false,true,true,true,false,false},{true,true,true,false,true,false,true},{true,false,true,true,true,true,true}}};
	   float [] var_427 = {(-1.16266e-15F)};
	   double var_535 = (-7.58843e-58D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(276);
		CrcCheck.ToByteArray(this.var_72,b,"var_72");
		CrcCheck.ToByteArray(this.var_77,b,"var_77");
		CrcCheck.ToByteArray(this.var_98.GetChecksum(),b,"var_98.GetChecksum()");
		CrcCheck.ToByteArray(this.var_127.GetChecksum(),b,"var_127.GetChecksum()");
		CrcCheck.ToByteArray(this.var_376.GetChecksum(),b,"var_376.GetChecksum()");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_381[a0][a1][a2].GetChecksum(),b,"var_381" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_405[a0][a1][a2],b,"var_405" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_427[a0],b,"var_427" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_535,b,"var_535");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
