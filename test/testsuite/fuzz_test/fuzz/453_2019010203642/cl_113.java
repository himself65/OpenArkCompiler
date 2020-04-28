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

//SEED=205973598

//import java.util.zip.CRC32;

class cl_113 
{
	   int var_81 = (-751374861);
	   byte var_108 = (byte)(99);
	   float [][] var_237 = {{(-6.71945e-11F),(-6.03991e-16F),(-4.84868e-05F),(4.33974e-30F),(7.96902e+12F)}};
	   boolean var_301 = false;
	final   short var_322 = (short)(-21382);
	   float var_329 = (-1.03187e+23F);
	   long [][][] var_340 = {{{(5057738857728261356L),(5195249985633062576L),(-6023704061923021387L),(-8891276371742186670L)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(64);
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_237[a0][a1],b,"var_237" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_301,b,"var_301");
		CrcCheck.ToByteArray(this.var_322,b,"var_322");
		CrcCheck.ToByteArray(this.var_329,b,"var_329");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_340[a0][a1][a2],b,"var_340" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
