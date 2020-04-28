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

//SEED=538048686

//import java.util.zip.CRC32;

class cl_115 
{
	   cl_113 var_85 = new cl_113();
	   cl_100 var_91 = new cl_100();
	   cl_123 var_92 = new cl_123();
	   byte var_98 = (byte)(116);
	   float var_105 = (3.13712e-32F);
	   int var_166 = (1014137424);
	   double [][] var_235 = {{(2.57927e-32D),(2.7759e+253D)},{(-2.3786e+244D),(6.8058e+214D)},{(2.4977e-296D),(-1.13583e+78D)},{(7.29805e-26D),(-4.32351e+175D)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(97);
		CrcCheck.ToByteArray(this.var_85.GetChecksum(),b,"var_85.GetChecksum()");
		CrcCheck.ToByteArray(this.var_91.GetChecksum(),b,"var_91.GetChecksum()");
		CrcCheck.ToByteArray(this.var_92.GetChecksum(),b,"var_92.GetChecksum()");
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		CrcCheck.ToByteArray(this.var_166,b,"var_166");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_235[a0][a1],b,"var_235" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
