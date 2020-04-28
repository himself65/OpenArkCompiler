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

class cl_82 extends cl_20
{
	   cl_20 var_58 = new cl_20();
	   cl_22 var_64 = new cl_22();
	   cl_66 var_76 = new cl_66();
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(54);
		CrcCheck.ToByteArray(this.var_58.GetChecksum(),b,"var_58.GetChecksum()");
		CrcCheck.ToByteArray(this.var_62,b,"var_62");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_63[a0],b,"var_63" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_64.GetChecksum(),b,"var_64.GetChecksum()");
		CrcCheck.ToByteArray(this.var_71,b,"var_71");
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_76.GetChecksum(),b,"var_76.GetChecksum()");
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_100,b,"var_100");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_111[a0][a1],b,"var_111" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
