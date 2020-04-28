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

//SEED=2776528782

//import java.util.zip.CRC32;

class cl_25 
{
	   boolean var_54 = false;
	  static byte var_56 = (byte)(-92);
	   int var_78 = (-892767348);
	  static float var_83 = (9.72006e-28F);
	  static short var_90 = (short)(-10036);
	   double var_146 = (-7.73368e-52D);
	   boolean [] var_176 = {false};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(21);
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		CrcCheck.ToByteArray(this.var_78,b,"var_78");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CrcCheck.ToByteArray(this.var_90,b,"var_90");
		CrcCheck.ToByteArray(this.var_146,b,"var_146");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_176[a0],b,"var_176" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
