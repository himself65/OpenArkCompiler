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

//SEED=2981708336

//import java.util.zip.CRC32;

class cl_10 
{
	   int var_46 = (-1769337376);
	   float var_107 = (-7.27915e+14F);
	   byte var_114 = (byte)(11);
	   short var_138 = (short)(30696);
	   boolean var_204 = false;
	final   long var_206 = (-9044540790801569556L);
	   float [] var_237 = {(1.74002e+09F)};
	   int [] var_385 = {(-1428108945)};
	final   double var_451 = (3.29145e-211D);
	   double var_507 = (7.80676e-45D);
	   long var_514 = (-262882702836080783L);
	final   float [][][] var_573 = {{{(-1.18149e-36F),(-4.68443e+30F),(1.15372e+25F),(-5.27552e+35F)}},{{(1.29943e+26F),(7.82712e-19F),(2.56408e-13F),(-8.1485e-29F)}},{{(-0.00650665F),(-6995.35F),(-6.17282e-06F),(-1.96634e+36F)}},{{(6.5373e+27F),(2.53003e+09F),(1.83791e+09F),(-0.223272F)}},{{(-1.85045e+11F),(8.4709e-30F),(2.31055e-11F),(4.98957e-22F)}},{{(3.41789e+26F),(58.2988F),(-7.67489e+30F),(4.07451e-20F)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(148);
		CrcCheck.ToByteArray(this.var_46,b,"var_46");
		CrcCheck.ToByteArray(this.var_107,b,"var_107");
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CrcCheck.ToByteArray(this.var_138,b,"var_138");
		CrcCheck.ToByteArray(this.var_204,b,"var_204");
		CrcCheck.ToByteArray(this.var_206,b,"var_206");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_237[a0],b,"var_237" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_385[a0],b,"var_385" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_451,b,"var_451");
		CrcCheck.ToByteArray(this.var_507,b,"var_507");
		CrcCheck.ToByteArray(this.var_514,b,"var_514");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_573[a0][a1][a2],b,"var_573" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
