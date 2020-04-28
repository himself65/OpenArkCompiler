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

//SEED=1360628436

import java.util.zip.CRC32;

class cl_44 
{
	   int [][] var_51 = {{(858466370),(-1689402372),(-407773764),(1298184552)},{(-1525787609),(1800448449),(1357783454),(-1728217368)},{(-1959666023),(-221706303),(-1011543760),(-2069636836)},{(-1958818183),(913289928),(-1608163524),(374196145)}};
	   boolean var_58 = true;
	   byte var_66 = (byte)(13);
	   short var_70 = (short)(11172);
	   boolean [] var_85 = {false,false};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(70);
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_51[a0][a1],b,"var_51" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		CrcCheck.ToByteArray(this.var_66,b,"var_66");
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		for(int a0=0;a0<2;++a0){
			CrcCheck.ToByteArray(this.var_85[a0],b,"var_85" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
