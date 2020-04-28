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

//SEED=2472495084

//import java.util.zip.CRC32;

class cl_46 extends cl_26
{
	   cl_26 var_131 = new cl_26();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(648);
		CrcCheck.ToByteArray(this.var_44,b,"var_44");
		CrcCheck.ToByteArray(this.var_45,b,"var_45");
		CrcCheck.ToByteArray(this.var_46,b,"var_46");
		CrcCheck.ToByteArray(this.var_47,b,"var_47");
		CrcCheck.ToByteArray(this.var_48,b,"var_48");
		CrcCheck.ToByteArray(this.var_49.GetChecksum(),b,"var_49.GetChecksum()");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_53[a0][a1],b,"var_53" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_56[a0],b,"var_56" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_60,b,"var_60");
		CrcCheck.ToByteArray(this.var_66,b,"var_66");
		CrcCheck.ToByteArray(this.var_67.GetChecksum(),b,"var_67.GetChecksum()");
		CrcCheck.ToByteArray(this.var_69,b,"var_69");
		CrcCheck.ToByteArray(this.var_72.GetChecksum(),b,"var_72.GetChecksum()");
		CrcCheck.ToByteArray(this.var_78,b,"var_78");
		CrcCheck.ToByteArray(this.var_79,b,"var_79");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_94[a0],b,"var_94" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_99[a0][a1].GetChecksum(),b,"var_99" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_104,b,"var_104");
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		CrcCheck.ToByteArray(this.var_131.GetChecksum(),b,"var_131.GetChecksum()");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<6;++a1){
			CrcCheck.ToByteArray(this.var_205[a0][a1].GetChecksum(),b,"var_205" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
