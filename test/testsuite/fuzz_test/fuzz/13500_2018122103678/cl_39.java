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

class cl_39 
{
	   int var_36 = (-505733947);
	   cl_46 var_55 = new cl_46();
	   cl_24 var_93 = new cl_24();
	   cl_81 [][] var_158 = {{new cl_81(),new cl_81()},{new cl_81(),new cl_81()},{new cl_81(),new cl_81()},{new cl_81(),new cl_81()},{new cl_81(),new cl_81()}};
	final   short var_180 = (short)(2681);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(102);
		CrcCheck.ToByteArray(this.var_36,b,"var_36");
		CrcCheck.ToByteArray(this.var_55.GetChecksum(),b,"var_55.GetChecksum()");
		CrcCheck.ToByteArray(this.var_93.GetChecksum(),b,"var_93.GetChecksum()");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_158[a0][a1].GetChecksum(),b,"var_158" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_180,b,"var_180");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
