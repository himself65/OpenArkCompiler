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
class cl_26 
{
	   short var_34 = (short)(-17369);
	   double var_103 = (3.02197e+113D);
	   long var_105 = (1769044878848318895L);
	   boolean [] var_118 = {false,false,true,true,false,true,true};
	   boolean var_218 = false;
/*********************************/
	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(26);
		CrcCheck.ToByteArray(this.var_34,b,"var_34");
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_118[a0],b,"var_118" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_218,b,"var_218");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
