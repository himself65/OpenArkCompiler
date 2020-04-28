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

//SEED=59004558

//import java.util.zip.CRC32;

class cl_39 extends cl_26
{
	final   cl_21 var_56 = new cl_21();
	   cl_26 var_64 = new cl_26();
	   cl_21 var_83 = new cl_21();
	   cl_23 var_109 = new cl_23();
	   cl_26 [][] var_162 = {{new cl_26(),new cl_26(),new cl_26(),new cl_26()}};
	   short var_202 = (short)(1988);
	   cl_26 [][][] var_221 = {{{new cl_26(),new cl_26(),new cl_26(),new cl_26(),new cl_26()}},{{new cl_26(),new cl_26(),new cl_26(),new cl_26(),new cl_26()}},{{new cl_26(),new cl_26(),new cl_26(),new cl_26(),new cl_26()}},{{new cl_26(),new cl_26(),new cl_26(),new cl_26(),new cl_26()}},{{new cl_26(),new cl_26(),new cl_26(),new cl_26(),new cl_26()}},{{new cl_26(),new cl_26(),new cl_26(),new cl_26(),new cl_26()}},{{new cl_26(),new cl_26(),new cl_26(),new cl_26(),new cl_26()}}};
	   boolean [][] var_573 = {{true,false,true,true,false}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(440);
		CrcCheck.ToByteArray(this.var_56.GetChecksum(),b,"var_56.GetChecksum()");
		CrcCheck.ToByteArray(this.var_60,b,"var_60");
		CrcCheck.ToByteArray(this.var_64.GetChecksum(),b,"var_64.GetChecksum()");
		CrcCheck.ToByteArray(this.var_83.GetChecksum(),b,"var_83.GetChecksum()");
		CrcCheck.ToByteArray(this.var_109.GetChecksum(),b,"var_109.GetChecksum()");
		CrcCheck.ToByteArray(this.var_111,b,"var_111");
		CrcCheck.ToByteArray(this.var_125,b,"var_125");
		CrcCheck.ToByteArray(this.var_145,b,"var_145");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_162[a0][a1].GetChecksum(),b,"var_162" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_202,b,"var_202");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_221[a0][a1][a2].GetChecksum(),b,"var_221" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_299,b,"var_299");
		CrcCheck.ToByteArray(this.var_369,b,"var_369");
		CrcCheck.ToByteArray(this.var_386,b,"var_386");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<3;++a2){
			CrcCheck.ToByteArray(this.var_479[a0][a1][a2],b,"var_479" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_500,b,"var_500");
		CrcCheck.ToByteArray(this.var_569,b,"var_569");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_573[a0][a1],b,"var_573" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
