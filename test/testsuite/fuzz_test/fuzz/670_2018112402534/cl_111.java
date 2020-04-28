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

class cl_111 
{
	   float var_102 = (-4.24332e-16F);
	final   cl_80 var_129 = new cl_80();
	   short var_169 = (short)(22927);
	final   boolean var_170 = false;
	   cl_32 var_254 = new cl_32();
	final   float [] var_261 = {(1.2961e+15F)};
	   int [] var_397 = {(1921407810)};
	   double var_409 = (-3.40989e-151D);
	   cl_14 var_467 = new cl_14();
	   int var_524 = (-735163028);
	   cl_10 var_572 = new cl_10();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(59);
		CrcCheck.ToByteArray(this.var_102,b,"var_102");
		CrcCheck.ToByteArray(this.var_129.GetChecksum(),b,"var_129.GetChecksum()");
		CrcCheck.ToByteArray(this.var_169,b,"var_169");
		CrcCheck.ToByteArray(this.var_170,b,"var_170");
		CrcCheck.ToByteArray(this.var_254.GetChecksum(),b,"var_254.GetChecksum()");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_261[a0],b,"var_261" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_397[a0],b,"var_397" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_409,b,"var_409");
		CrcCheck.ToByteArray(this.var_467.GetChecksum(),b,"var_467.GetChecksum()");
		CrcCheck.ToByteArray(this.var_524,b,"var_524");
		CrcCheck.ToByteArray(this.var_572.GetChecksum(),b,"var_572.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
