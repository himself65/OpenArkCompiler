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

//SEED=269244036

import java.util.zip.CRC32;

class cl_35 
{
	   byte var_40 = (byte)(14);
	   int [][] var_48 = {{(382682512),(-199405722)},{(1754397942),(-1321786930)},{(-1017107822),(-52190277)},{(-1483275097),(-1504470813)}};
	   short var_59 = (short)(-6819);
	   boolean [] var_60 = {false,false,true,true,true,true,true};
	   long var_80 = (-6583896584871310622L);
	   double var_96 = (-3.96105e+210D);
	   int var_106 = (1311394830);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(62);
		CrcCheck.ToByteArray(this.var_40,b,"var_40");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_48[a0][a1],b,"var_48" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_60[a0],b,"var_60" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_80,b,"var_80");
		CrcCheck.ToByteArray(this.var_96,b,"var_96");
		CrcCheck.ToByteArray(this.var_106,b,"var_106");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
