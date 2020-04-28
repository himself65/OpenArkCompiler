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
class cl_108 
{
	   long var_76 = (-4472109714255938465L);
	   boolean var_102 = true;
	   int var_117 = (513694508);
	   short var_149 = (short)(19494);
	   byte var_193 = (byte)(-42);
	   boolean [] var_196 = {true,true,false};
/*********************************/
	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(19);
		CrcCheck.ToByteArray(this.var_76,b,"var_76");
		CrcCheck.ToByteArray(this.var_102,b,"var_102");
		CrcCheck.ToByteArray(this.var_117,b,"var_117");
		CrcCheck.ToByteArray(this.var_149,b,"var_149");
		CrcCheck.ToByteArray(this.var_193,b,"var_193");
		for(int a0=0;a0<3;++a0){
			CrcCheck.ToByteArray(this.var_196[a0],b,"var_196" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
