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

class cl_52 extends cl_21
{
	final   cl_21 var_58 = new cl_21();
	   cl_21 var_61 = new cl_21();
	   cl_60 var_68 = new cl_60();
	final   cl_86 var_80 = new cl_86();
	final   double var_109 = (1.72489e+90D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(396);
		CrcCheck.ToByteArray(this.var_58.GetChecksum(),b,"var_58.GetChecksum()");
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_61.GetChecksum(),b,"var_61.GetChecksum()");
		CrcCheck.ToByteArray(this.var_68.GetChecksum(),b,"var_68.GetChecksum()");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_74[a0],b,"var_74" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_80.GetChecksum(),b,"var_80.GetChecksum()");
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		CrcCheck.ToByteArray(this.var_107,b,"var_107");
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
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
