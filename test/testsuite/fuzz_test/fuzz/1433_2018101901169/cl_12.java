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

//SEED=3798900212

import java.util.zip.CRC32;

class cl_12 
{
	   short var_28 = (short)(28055);
	   float var_40 = (10.7531F);
	   int var_58 = (1598268895);
	final   byte var_68 = (byte)(-74);
	   int [][] var_101 = {{(-533542133),(-1360744183),(-267446977)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(23);
		CrcCheck.ToByteArray(this.var_28,b,"var_28");
		CrcCheck.ToByteArray(this.var_40,b,"var_40");
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_101[a0][a1],b,"var_101" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
