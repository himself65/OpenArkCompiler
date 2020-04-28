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

class cl_75 extends cl_22
{
	   cl_22 var_51 = new cl_22();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(352);
		CrcCheck.ToByteArray(this.var_51.GetChecksum(),b,"var_51.GetChecksum()");
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_60[a0][a1],b,"var_60" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_77,b,"var_77");
		CrcCheck.ToByteArray(this.var_79,b,"var_79");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<6;++a1){
			CrcCheck.ToByteArray(this.var_85[a0][a1],b,"var_85" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_104[a0][a1],b,"var_104" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_228[a0][a1][a2],b,"var_228" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_233[a0][a1],b,"var_233" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
