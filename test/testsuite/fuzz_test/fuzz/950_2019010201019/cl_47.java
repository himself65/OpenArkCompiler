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

//SEED=891913062

//import java.util.zip.CRC32;

class cl_47 extends cl_26
{
	   cl_26 var_113 = new cl_26();
	final   double [][][] var_227 = {{{(1.69356e-68D)},{(-1.8527e-37D)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(244);
		CrcCheck.ToByteArray(this.var_44,b,"var_44");
		CrcCheck.ToByteArray(this.var_47.GetChecksum(),b,"var_47.GetChecksum()");
		CrcCheck.ToByteArray(this.var_50.GetChecksum(),b,"var_50.GetChecksum()");
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		CrcCheck.ToByteArray(this.var_57.GetChecksum(),b,"var_57.GetChecksum()");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_58[a0][a1],b,"var_58" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_61.GetChecksum(),b,"var_61.GetChecksum()");
		CrcCheck.ToByteArray(this.var_62,b,"var_62");
		CrcCheck.ToByteArray(this.var_63.GetChecksum(),b,"var_63.GetChecksum()");
		CrcCheck.ToByteArray(this.var_66,b,"var_66");
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		CrcCheck.ToByteArray(this.var_80,b,"var_80");
		CrcCheck.ToByteArray(this.var_84,b,"var_84");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<1;++a1){
			CrcCheck.ToByteArray(this.var_89[a0][a1],b,"var_89" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_91[a0][a1][a2],b,"var_91" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_97,b,"var_97");
		CrcCheck.ToByteArray(this.var_100,b,"var_100");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		CrcCheck.ToByteArray(this.var_113.GetChecksum(),b,"var_113.GetChecksum()");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_227[a0][a1][a2],b,"var_227" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
