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

class cl_46 extends cl_24
{
	   cl_24 var_56 = new cl_24();
	   cl_81 var_75 = new cl_81();
	   cl_89 var_154 = new cl_89();
	   long var_232 = (5261615551694280407L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(753);
		CrcCheck.ToByteArray(this.var_23,b,"var_23");
		CrcCheck.ToByteArray(this.var_56.GetChecksum(),b,"var_56.GetChecksum()");
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_69[a0][a1][a2],b,"var_69" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_75.GetChecksum(),b,"var_75.GetChecksum()");
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_117[a0],b,"var_117" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_122,b,"var_122");
		CrcCheck.ToByteArray(this.var_123,b,"var_123");
		CrcCheck.ToByteArray(this.var_154.GetChecksum(),b,"var_154.GetChecksum()");
		CrcCheck.ToByteArray(this.var_232,b,"var_232");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_234[a0][a1][a2],b,"var_234" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
