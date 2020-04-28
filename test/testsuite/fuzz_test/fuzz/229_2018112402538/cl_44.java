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

class cl_44 
{
	   cl_39 var_63 = new cl_39();
	final   cl_21 var_77 = new cl_21();
	   cl_21 var_79 = new cl_21();
	   short var_230 = (short)(27680);
	   double var_256 = (1.05972e-249D);
	   cl_23 var_268 = new cl_23();
	final   int var_326 = (-1183844900);
	   boolean var_334 = true;
	final   byte var_425 = (byte)(-72);
	   boolean [][] var_576 = {{false,false,false,true,false}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(53);
		CrcCheck.ToByteArray(this.var_63.GetChecksum(),b,"var_63.GetChecksum()");
		CrcCheck.ToByteArray(this.var_77.GetChecksum(),b,"var_77.GetChecksum()");
		CrcCheck.ToByteArray(this.var_79.GetChecksum(),b,"var_79.GetChecksum()");
		CrcCheck.ToByteArray(this.var_230,b,"var_230");
		CrcCheck.ToByteArray(this.var_256,b,"var_256");
		CrcCheck.ToByteArray(this.var_268.GetChecksum(),b,"var_268.GetChecksum()");
		CrcCheck.ToByteArray(this.var_326,b,"var_326");
		CrcCheck.ToByteArray(this.var_334,b,"var_334");
		CrcCheck.ToByteArray(this.var_425,b,"var_425");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_576[a0][a1],b,"var_576" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
