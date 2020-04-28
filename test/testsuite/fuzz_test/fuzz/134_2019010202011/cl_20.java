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

//SEED=3331378896

//import java.util.zip.CRC32;

class cl_20 
{
	   long var_62 = (1440659960753226582L);
	final   boolean [] var_63 = {true,false,false,false};
	   byte var_71 = (byte)(-72);
	   short var_75 = (short)(7623);
	   int var_81 = (2139880156);
	   boolean var_100 = false;
	   float var_108 = (-2.65501e+35F);
	   boolean [][] var_111 = {{true,false},{true,true},{true,true}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(30);
		CrcCheck.ToByteArray(this.var_62,b,"var_62");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_63[a0],b,"var_63" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_71,b,"var_71");
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_100,b,"var_100");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_111[a0][a1],b,"var_111" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
