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

//SEED=4271641260

//import java.util.zip.CRC32;

class cl_54 
{
	   cl_24 var_44 = new cl_24();
	   cl_3 var_103 = new cl_3();
	   cl_3 [][][] var_111 = {{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}}};
	final   cl_10 var_153 = new cl_10();
	   short var_205 = (short)(26120);
	final   byte var_236 = (byte)(-102);
	   float var_241 = (-1.38922e-26F);
	   boolean var_252 = false;
	   cl_10 var_262 = new cl_10();
	   int var_323 = (-968967408);
	   cl_3 [][] var_342 = {{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}};
	   double var_471 = (6.87617e+292D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(2212);
		CrcCheck.ToByteArray(this.var_44.GetChecksum(),b,"var_44.GetChecksum()");
		CrcCheck.ToByteArray(this.var_103.GetChecksum(),b,"var_103.GetChecksum()");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_111[a0][a1][a2].GetChecksum(),b,"var_111" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_153.GetChecksum(),b,"var_153.GetChecksum()");
		CrcCheck.ToByteArray(this.var_205,b,"var_205");
		CrcCheck.ToByteArray(this.var_236,b,"var_236");
		CrcCheck.ToByteArray(this.var_241,b,"var_241");
		CrcCheck.ToByteArray(this.var_252,b,"var_252");
		CrcCheck.ToByteArray(this.var_262.GetChecksum(),b,"var_262.GetChecksum()");
		CrcCheck.ToByteArray(this.var_323,b,"var_323");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<6;++a1){
			CrcCheck.ToByteArray(this.var_342[a0][a1].GetChecksum(),b,"var_342" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_471,b,"var_471");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
