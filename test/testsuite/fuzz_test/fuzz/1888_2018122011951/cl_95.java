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
class cl_95 
{
	final   boolean [] var_107 = {true,true,false,false,false,false,true};
	   byte var_134 = (byte)(35);
	   float var_138 = (11792F);
	   long var_163 = (5186932966959138666L);
	final   short var_184 = (short)(17429);
/*********************************/
	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(22);
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_107[a0],b,"var_107" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_134,b,"var_134");
		CrcCheck.ToByteArray(this.var_138,b,"var_138");
		CrcCheck.ToByteArray(this.var_163,b,"var_163");
		CrcCheck.ToByteArray(this.var_184,b,"var_184");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
