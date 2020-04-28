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

//SEED=538048686

//import java.util.zip.CRC32;

class cl_123 
{
	   long [][] var_90 = {{(5229752552271110253L),(-4913309046452838073L),(5162240367647145848L),(4264290875097534268L),(-2104496683475282040L)},{(4374772185759916630L),(-306836400417222913L),(5978157413040034736L),(-8950139927706962545L),(-4283975254837504116L)},{(5304700131676707682L),(1839753771477876542L),(-780768483200617518L),(6484492641573390697L),(2268124448074484492L)},{(-3519906890958711609L),(2322483596080656289L),(-7117514191930190687L),(-3277847815941057150L),(742217670321962958L)},{(3932217135429026326L),(-1449404999287174821L),(485621642044506764L),(-6754858427034741879L),(3970657973969034822L)},{(411002072395751583L),(9206904977079795498L),(-2346325529490436030L),(-358649888272058498L),(-1464769798455735705L)},{(-8941099245224252212L),(-5646697487811543468L),(8073543830630643023L),(2787138011350284874L),(-1110250860840070412L)}};
	final   double var_93 = (3.60666e-101D);
	   boolean var_94 = false;
	   long var_96 = (4432952396820741873L);
	   double var_106 = (-2.93053e-226D);
	   short var_112 = (short)(-11915);
	   int var_164 = (18591877);
	   double [] var_169 = {(-1.24193e-37D)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(319);
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_90[a0][a1],b,"var_90" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_93,b,"var_93");
		CrcCheck.ToByteArray(this.var_94,b,"var_94");
		CrcCheck.ToByteArray(this.var_96,b,"var_96");
		CrcCheck.ToByteArray(this.var_106,b,"var_106");
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		CrcCheck.ToByteArray(this.var_164,b,"var_164");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_169[a0],b,"var_169" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
