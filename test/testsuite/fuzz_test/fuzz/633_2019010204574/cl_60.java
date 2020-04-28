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

//SEED=2472495084

//import java.util.zip.CRC32;

class cl_60 
{
	   int var_50 = (1238006085);
	final   short var_55 = (short)(-19982);
	   float var_57 = (101687F);
	   byte var_62 = (byte)(64);
	   double var_70 = (-4.9034e-137D);
	final   long var_88 = (-5398573418839455762L);
	   int [] var_96 = {(-1516426588),(-437723701),(-1006618566),(479290235)};
	   long var_98 = (-5186097583038485892L);
	final   float [][] var_108 = {{(4.64255e+07F)},{(-3.82713e+19F)},{(8.96726e-27F)},{(-6.68279e+23F)},{(-8.88204e+08F)},{(4.48372e-35F)},{(1.90221e+13F)}};
	   float [] var_188 = {(-3.34788e+31F)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(83);
		CrcCheck.ToByteArray(this.var_50,b,"var_50");
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_57,b,"var_57");
		CrcCheck.ToByteArray(this.var_62,b,"var_62");
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_88,b,"var_88");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_96[a0],b,"var_96" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<1;++a1){
			CrcCheck.ToByteArray(this.var_108[a0][a1],b,"var_108" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_188[a0],b,"var_188" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
