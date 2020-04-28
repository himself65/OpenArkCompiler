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

//SEED=1354424452

import java.util.zip.CRC32;

class cl_9 
{
	   long var_25 = (-8354287147659806612L);
	   double var_39 = (5.68016e-105D);
	final   int var_45 = (874191798);
	   short var_58 = (short)(18832);
	   boolean var_93 = true;
	final   long [][][] var_94 = {{{(-642071665996897700L),(3526089245913446329L),(1944248263834714736L),(8494613142375998799L)},{(-6860766601141274728L),(6127138418034300087L),(-3509748334345517709L),(4521792607050053185L)}}};
	final   byte var_122 = (byte)(-14);
	   float var_168 = (2.31684e-36F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(92);
		CrcCheck.ToByteArray(this.var_25,b,"var_25");
		CrcCheck.ToByteArray(this.var_39,b,"var_39");
		CrcCheck.ToByteArray(this.var_45,b,"var_45");
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		CrcCheck.ToByteArray(this.var_93,b,"var_93");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_94[a0][a1][a2],b,"var_94" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_122,b,"var_122");
		CrcCheck.ToByteArray(this.var_168,b,"var_168");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
