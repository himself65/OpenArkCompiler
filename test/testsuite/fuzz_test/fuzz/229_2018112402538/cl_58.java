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

class cl_58 
{
	   cl_44 var_68 = new cl_44();
	final   cl_21 var_186 = new cl_21();
	final   cl_23 var_194 = new cl_23();
	   cl_21 var_265 = new cl_21();
	   cl_23 var_275 = new cl_23();
	   cl_26 var_298 = new cl_26();
	   cl_39 var_309 = new cl_39();
	   boolean var_379 = false;
	final   int var_498 = (-1508524096);
	   boolean [][] var_574 = {{false,false,true,false,true}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(66);
		CrcCheck.ToByteArray(this.var_68.GetChecksum(),b,"var_68.GetChecksum()");
		CrcCheck.ToByteArray(this.var_186.GetChecksum(),b,"var_186.GetChecksum()");
		CrcCheck.ToByteArray(this.var_194.GetChecksum(),b,"var_194.GetChecksum()");
		CrcCheck.ToByteArray(this.var_265.GetChecksum(),b,"var_265.GetChecksum()");
		CrcCheck.ToByteArray(this.var_275.GetChecksum(),b,"var_275.GetChecksum()");
		CrcCheck.ToByteArray(this.var_298.GetChecksum(),b,"var_298.GetChecksum()");
		CrcCheck.ToByteArray(this.var_309.GetChecksum(),b,"var_309.GetChecksum()");
		CrcCheck.ToByteArray(this.var_379,b,"var_379");
		CrcCheck.ToByteArray(this.var_498,b,"var_498");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_574[a0][a1],b,"var_574" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
