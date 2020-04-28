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

//SEED=1902118766

//import java.util.zip.CRC32;

class cl_41 
{
	   int var_50 = (-441227531);
	   byte var_55 = (byte)(6);
	final   cl_35 var_59 = new cl_35();
	   cl_4 var_73 = new cl_4();
	   cl_7 var_76 = new cl_7();
	   cl_30 var_94 = new cl_30();
	   boolean var_149 = false;
	   cl_35 [][][] var_178 = {{{new cl_35(),new cl_35(),new cl_35(),new cl_35()},{new cl_35(),new cl_35(),new cl_35(),new cl_35()}},{{new cl_35(),new cl_35(),new cl_35(),new cl_35()},{new cl_35(),new cl_35(),new cl_35(),new cl_35()}},{{new cl_35(),new cl_35(),new cl_35(),new cl_35()},{new cl_35(),new cl_35(),new cl_35(),new cl_35()}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(230);
		CrcCheck.ToByteArray(this.var_50,b,"var_50");
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_59.GetChecksum(),b,"var_59.GetChecksum()");
		CrcCheck.ToByteArray(this.var_73.GetChecksum(),b,"var_73.GetChecksum()");
		CrcCheck.ToByteArray(this.var_76.GetChecksum(),b,"var_76.GetChecksum()");
		CrcCheck.ToByteArray(this.var_94.GetChecksum(),b,"var_94.GetChecksum()");
		CrcCheck.ToByteArray(this.var_149,b,"var_149");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_178[a0][a1][a2].GetChecksum(),b,"var_178" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
