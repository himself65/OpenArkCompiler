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

class cl_32 extends cl_14
{
	   cl_14 var_50 = new cl_14();
	   cl_10 var_113 = new cl_10();
	   boolean var_193 = false;
	   long [] var_377 = {(997407672304979199L),(2104717096730304540L),(7733000571095221454L),(5626803798121845247L),(8570103398066005430L),(5266934066860018096L),(-786859617622926668L)};
	   float [] var_446 = {(-98.921F)};
	   cl_10 [][] var_517 = {{new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10()},{new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10()},{new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10()},{new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10()},{new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10()},{new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10(),new cl_10()}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(583);
		CrcCheck.ToByteArray(this.var_32,b,"var_32");
		CrcCheck.ToByteArray(this.var_50.GetChecksum(),b,"var_50.GetChecksum()");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		CrcCheck.ToByteArray(this.var_113.GetChecksum(),b,"var_113.GetChecksum()");
		CrcCheck.ToByteArray(this.var_148,b,"var_148");
		CrcCheck.ToByteArray(this.var_193,b,"var_193");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_212[a0][a1][a2],b,"var_212" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_230,b,"var_230");
		CrcCheck.ToByteArray(this.var_232,b,"var_232");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_377[a0],b,"var_377" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_417[a0],b,"var_417" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_446[a0],b,"var_446" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_517[a0][a1].GetChecksum(),b,"var_517" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_591[a0][a1][a2],b,"var_591" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_621,b,"var_621");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
