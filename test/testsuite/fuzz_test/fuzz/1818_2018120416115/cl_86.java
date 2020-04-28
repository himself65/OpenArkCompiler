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

//SEED=1973468952

//import java.util.zip.CRC32;

class cl_86 extends cl_21
{
	   cl_21 var_65 = new cl_21();
	   cl_60 var_86 = new cl_60();
	   boolean [][][] var_432 = {{{false,false,true,true,false,false,true},{false,true,true,false,true,true,true}},{{true,false,true,false,false,false,false},{false,false,true,false,false,true,true}},{{true,true,true,true,false,true,true},{true,false,true,false,false,true,true}},{{true,true,true,false,true,true,true},{true,true,false,true,false,true,false}},{{false,true,true,true,false,false,true},{true,true,false,false,false,false,true}},{{true,false,true,false,true,true,false},{true,true,false,true,true,false,false}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(456);
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_65.GetChecksum(),b,"var_65.GetChecksum()");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_74[a0],b,"var_74" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_86.GetChecksum(),b,"var_86.GetChecksum()");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		CrcCheck.ToByteArray(this.var_107,b,"var_107");
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		CrcCheck.ToByteArray(this.var_136,b,"var_136");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_268[a0][a1],b,"var_268" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_432[a0][a1][a2],b,"var_432" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_433[a0][a1][a2],b,"var_433" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
