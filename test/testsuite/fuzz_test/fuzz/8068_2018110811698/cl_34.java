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

//SEED=2069637170

import java.util.zip.CRC32;

class cl_34 
{
	   byte var_26 = (byte)(-6);
	   short var_43 = (short)(25942);
	   boolean [][][] var_46 = {{{true,true,true},{false,true,false},{true,false,true},{true,false,false},{false,false,false},{true,true,true},{false,false,false}}};
	   boolean [][] var_53 = {{true,true,true,false,false},{false,true,false,false,true}};
	final   double var_54 = (1.07083e-237D);
	final   double [][] var_87 = {{(-3.28007e-17D)},{(3.24734e-212D)},{(3.58298e-132D)},{(1.31852e-42D)},{(-2.83527e+293D)},{(-3.45904e+158D)}};
	final   int var_105 = (310558442);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(94);
		CrcCheck.ToByteArray(this.var_26,b,"var_26");
		CrcCheck.ToByteArray(this.var_43,b,"var_43");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<3;++a2){
			CrcCheck.ToByteArray(this.var_46[a0][a1][a2],b,"var_46" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_53[a0][a1],b,"var_53" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<1;++a1){
			CrcCheck.ToByteArray(this.var_87[a0][a1],b,"var_87" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
