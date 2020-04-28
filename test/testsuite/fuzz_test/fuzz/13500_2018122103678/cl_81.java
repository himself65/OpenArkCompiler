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

//SEED=1157171100

//import java.util.zip.CRC32;

class cl_81 
{
	   double var_73 = (-4.79764e+154D);
	   int var_74 = (1767925968);
	   byte var_76 = (byte)(107);
	   long var_127 = (8475768886516257579L);
	   float var_184 = (-6.91734e-38F);
	final   boolean [][][] var_192 = {{{true,true,false,false,true,false,false},{false,false,false,true,false,false,true},{false,false,false,true,false,true,false},{false,false,true,false,false,false,true},{false,false,true,false,true,false,false},{false,false,true,true,true,false,false},{false,true,true,false,true,false,true}},{{true,true,true,false,false,true,true},{false,true,false,false,true,true,false},{false,false,false,true,false,false,false},{true,false,true,false,false,false,false},{true,false,true,true,true,true,true},{false,true,true,false,false,true,true},{false,false,false,true,true,false,false}},{{false,false,false,true,false,true,false},{false,true,false,false,true,true,false},{false,true,true,true,true,true,false},{true,false,false,true,true,false,true},{false,false,false,true,false,true,false},{false,false,true,true,true,true,true},{false,true,true,true,true,false,false}},{{true,true,false,true,false,false,false},{false,false,false,false,false,true,false},{true,true,true,false,false,true,false},{false,true,false,false,true,false,false},{false,true,true,false,false,true,true},{false,true,false,true,true,true,false},{false,true,false,false,true,true,false}},{{true,true,false,false,true,false,false},{false,false,true,false,false,false,false},{false,false,false,false,false,false,false},{true,true,true,true,false,false,false},{false,false,true,false,false,false,true},{false,false,true,false,true,false,false},{false,true,false,true,true,true,true}},{{true,false,false,false,true,true,true},{true,false,true,true,false,true,false},{true,false,false,false,false,false,true},{true,false,false,false,false,true,true},{true,true,true,true,false,true,true},{false,true,true,true,true,true,false},{false,false,true,false,true,true,false}}};
	   short var_212 = (short)(-1367);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(321);
		CrcCheck.ToByteArray(this.var_73,b,"var_73");
		CrcCheck.ToByteArray(this.var_74,b,"var_74");
		CrcCheck.ToByteArray(this.var_76,b,"var_76");
		CrcCheck.ToByteArray(this.var_127,b,"var_127");
		CrcCheck.ToByteArray(this.var_184,b,"var_184");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_192[a0][a1][a2],b,"var_192" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_212,b,"var_212");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
