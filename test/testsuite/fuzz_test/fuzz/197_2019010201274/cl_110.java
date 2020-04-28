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

class cl_110 extends cl_66
{
	   cl_66 var_75 = new cl_66();
	   long [] var_86 = {(-8093398084363872279L),(387715887705026735L),(7163302013071940138L),(3788770558072433940L),(7837099675896899906L),(-7738173715611188085L),(4089399776532554486L)};
	final   cl_83 var_90 = new cl_83();
	   cl_83 var_154 = new cl_83();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(170);
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_75.GetChecksum(),b,"var_75.GetChecksum()");
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_86[a0],b,"var_86" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_89[a0],b,"var_89" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_90.GetChecksum(),b,"var_90.GetChecksum()");
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
		CrcCheck.ToByteArray(this.var_154.GetChecksum(),b,"var_154.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
