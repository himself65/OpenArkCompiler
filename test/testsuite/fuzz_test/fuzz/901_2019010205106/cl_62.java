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

//SEED=562055798

//import java.util.zip.CRC32;

class cl_62 
{
	   int var_70 = (-1276667820);
	   boolean var_82 = true;
	   long [][][] var_152 = {{{(-898702243760780436L),(5876736178110465074L),(-6519475233774475400L)}},{{(-851705288654769878L),(1031812173926028730L),(-3046868964285581010L)}},{{(-759969941149653280L),(-7237429914075822952L),(-8823324555111280352L)}}};
	   byte var_219 = (byte)(28);
	   short var_234 = (short)(25807);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(80);
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_82,b,"var_82");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<3;++a2){
			CrcCheck.ToByteArray(this.var_152[a0][a1][a2],b,"var_152" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_219,b,"var_219");
		CrcCheck.ToByteArray(this.var_234,b,"var_234");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
