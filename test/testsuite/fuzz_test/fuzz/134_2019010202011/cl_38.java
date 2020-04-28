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

//SEED=3331378896

//import java.util.zip.CRC32;

class cl_38 extends cl_22
{
	   cl_22 var_33 = new cl_22();
	final   cl_82 var_59 = new cl_82();
	   cl_20 var_61 = new cl_20();
	   cl_66 var_82 = new cl_66();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(2793);
		CrcCheck.ToByteArray(this.var_33.GetChecksum(),b,"var_33.GetChecksum()");
		CrcCheck.ToByteArray(this.var_34,b,"var_34");
		CrcCheck.ToByteArray(this.var_59.GetChecksum(),b,"var_59.GetChecksum()");
		CrcCheck.ToByteArray(this.var_61.GetChecksum(),b,"var_61.GetChecksum()");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_65[a0],b,"var_65" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		CrcCheck.ToByteArray(this.var_82.GetChecksum(),b,"var_82.GetChecksum()");
		CrcCheck.ToByteArray(this.var_102,b,"var_102");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_143[a0][a1][a2],b,"var_143" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
