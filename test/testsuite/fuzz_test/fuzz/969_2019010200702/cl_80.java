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

//SEED=3943380884

//import java.util.zip.CRC32;

class cl_80 
{
	   long var_84 = (-4125777209256713976L);
	   int var_93 = (850737933);
	   double var_94 = (-3.40068e-55D);
	   float var_105 = (1.67416e+23F);
	   boolean [][] var_123 = {{true,true,true,false,false,true},{false,false,true,false,false,false},{true,true,true,true,false,true},{true,true,false,true,true,true},{false,false,true,false,true,false},{false,true,false,true,false,true},{true,false,true,false,false,true}};
	   short var_136 = (short)(30657);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(68);
		CrcCheck.ToByteArray(this.var_84,b,"var_84");
		CrcCheck.ToByteArray(this.var_93,b,"var_93");
		CrcCheck.ToByteArray(this.var_94,b,"var_94");
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<6;++a1){
			CrcCheck.ToByteArray(this.var_123[a0][a1],b,"var_123" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_136,b,"var_136");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
