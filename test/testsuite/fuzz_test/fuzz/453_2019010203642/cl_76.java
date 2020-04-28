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

//SEED=205973598

//import java.util.zip.CRC32;

class cl_76 
{
	   int var_65 = (570801366);
	   int [][][] var_72 = {{{(921844524)},{(650825937)},{(-421821818)}},{{(-543040609)},{(621844322)},{(-1749836892)}},{{(247998990)},{(1476475974)},{(-542117281)}},{{(1867831804)},{(1990739389)},{(-1430945029)}},{{(-2044699835)},{(-675179744)},{(1023572172)}}};
	   float var_83 = (-1.16923e-26F);
	   short var_87 = (short)(5716);
	   byte var_88 = (byte)(-39);
	final   double var_311 = (5.73663e+165D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(79);
		CrcCheck.ToByteArray(this.var_65,b,"var_65");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_72[a0][a1][a2],b,"var_72" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		CrcCheck.ToByteArray(this.var_87,b,"var_87");
		CrcCheck.ToByteArray(this.var_88,b,"var_88");
		CrcCheck.ToByteArray(this.var_311,b,"var_311");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
