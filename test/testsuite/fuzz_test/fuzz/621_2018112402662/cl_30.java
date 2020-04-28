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

//SEED=1902118766

//import java.util.zip.CRC32;

class cl_30 
{
	final   int var_86 = (-991030544);
	   boolean [][][] var_95 = {{{true,false,false,true,false,false},{true,true,true,false,false,true},{false,true,false,false,true,false},{false,true,true,true,true,true},{false,true,true,true,true,false},{true,true,true,true,false,true},{false,false,false,true,false,false}},{{false,true,true,true,true,true},{true,true,true,true,false,true},{true,false,true,true,true,false},{true,true,false,false,false,false},{false,true,true,true,false,true},{true,false,false,false,true,false},{true,false,true,true,false,false}},{{true,false,false,true,false,true},{false,false,false,true,true,true},{true,true,false,false,false,false},{false,true,true,false,false,true},{false,true,true,true,false,false},{true,false,true,true,false,false},{false,false,true,false,false,false}},{{false,false,true,false,false,true},{true,false,false,true,false,true},{false,false,false,true,false,true},{true,false,false,true,true,true},{false,false,false,false,true,true},{false,true,false,false,false,true},{false,true,false,true,true,false}}};
	   int var_100 = (2008552765);
	   byte var_113 = (byte)(-98);
	   float var_120 = (-4.28479e+11F);
	   long var_130 = (8106802808844173522L);
	   short var_197 = (short)(-26651);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(191);
		CrcCheck.ToByteArray(this.var_86,b,"var_86");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_95[a0][a1][a2],b,"var_95" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_100,b,"var_100");
		CrcCheck.ToByteArray(this.var_113,b,"var_113");
		CrcCheck.ToByteArray(this.var_120,b,"var_120");
		CrcCheck.ToByteArray(this.var_130,b,"var_130");
		CrcCheck.ToByteArray(this.var_197,b,"var_197");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
