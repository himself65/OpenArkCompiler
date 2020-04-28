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

//SEED=1595514112

import java.util.zip.CRC32;

class cl_9 
{
	   boolean var_7 = true;
	   long var_12 = (-2505716261215778601L);
	   float var_15 = (8.29438e-29F);
	   int [][] var_26 = {{(978419980),(1316470147),(977113513)},{(-1241433364),(23743312),(-1280934317)}};
	   double var_43 = (-4.24576e-165D);
	   int var_46 = (1635316722);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(49);
		CrcCheck.ToByteArray(this.var_7,b,"var_7");
		CrcCheck.ToByteArray(this.var_12,b,"var_12");
		CrcCheck.ToByteArray(this.var_15,b,"var_15");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_26[a0][a1],b,"var_26" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_43,b,"var_43");
		CrcCheck.ToByteArray(this.var_46,b,"var_46");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
