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

class cl_90 
{
	   float [] var_72 = {(0.0300555F)};
	   boolean var_88 = false;
	   double var_101 = (-1.86153e+243D);
	   cl_44 var_105 = new cl_44();
	   cl_21 var_106 = new cl_21();
	final   short var_116 = (short)(12378);
	   cl_52 var_163 = new cl_52();
	   cl_60 var_168 = new cl_60();
	   int var_287 = (1310183778);
	final   byte var_419 = (byte)(55);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(52);
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_72[a0],b,"var_72" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_88,b,"var_88");
		CrcCheck.ToByteArray(this.var_101,b,"var_101");
		CrcCheck.ToByteArray(this.var_105.GetChecksum(),b,"var_105.GetChecksum()");
		CrcCheck.ToByteArray(this.var_106.GetChecksum(),b,"var_106.GetChecksum()");
		CrcCheck.ToByteArray(this.var_116,b,"var_116");
		CrcCheck.ToByteArray(this.var_163.GetChecksum(),b,"var_163.GetChecksum()");
		CrcCheck.ToByteArray(this.var_168.GetChecksum(),b,"var_168.GetChecksum()");
		CrcCheck.ToByteArray(this.var_287,b,"var_287");
		CrcCheck.ToByteArray(this.var_419,b,"var_419");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
