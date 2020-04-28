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

class cl_97 
{
	   int var_68 = (829469791);
	   long var_81 = (7898830884379043813L);
	   byte var_87 = (byte)(107);
	   double var_90 = (-6.8113e+305D);
	final   int [] var_95 = {(-646460076),(-1410654674),(277615202),(-1272521998)};
	   boolean [] var_111 = {true,true,true,false,false};
	   short var_122 = (short)(14142);
	   float var_161 = (-4.99135e-29F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(48);
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_87,b,"var_87");
		CrcCheck.ToByteArray(this.var_90,b,"var_90");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_95[a0],b,"var_95" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_111[a0],b,"var_111" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_122,b,"var_122");
		CrcCheck.ToByteArray(this.var_161,b,"var_161");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
