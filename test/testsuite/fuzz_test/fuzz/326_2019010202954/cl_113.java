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

//SEED=538048686

//import java.util.zip.CRC32;

class cl_113 extends cl_100
{
	   cl_100 var_76 = new cl_100();
	   cl_123 var_89 = new cl_123();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(324);
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_76.GetChecksum(),b,"var_76.GetChecksum()");
		CrcCheck.ToByteArray(this.var_77,b,"var_77");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_88[a0][a1],b,"var_88" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_89.GetChecksum(),b,"var_89.GetChecksum()");
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_110,b,"var_110");
		CrcCheck.ToByteArray(this.var_113,b,"var_113");
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CrcCheck.ToByteArray(this.var_146,b,"var_146");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
