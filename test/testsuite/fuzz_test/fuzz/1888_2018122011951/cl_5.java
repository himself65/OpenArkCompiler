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

//SEED=1925474860
//import java.util.zip.CRC32;
class cl_5 
{
	   byte var_60 = (byte)(-74);
	   long var_78 = (5590839143230083488L);
	   float var_85 = (6.43939e+07F);
	   int var_86 = (2044368156);
	   boolean var_92 = true;
	   double var_100 = (-7.67493e+141D);
	final   boolean [] var_109 = {false,true,true,true,true,false,true};
/*********************************/
	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(33);
		CrcCheck.ToByteArray(this.var_60,b,"var_60");
		CrcCheck.ToByteArray(this.var_78,b,"var_78");
		CrcCheck.ToByteArray(this.var_85,b,"var_85");
		CrcCheck.ToByteArray(this.var_86,b,"var_86");
		CrcCheck.ToByteArray(this.var_92,b,"var_92");
		CrcCheck.ToByteArray(this.var_100,b,"var_100");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_109[a0],b,"var_109" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
