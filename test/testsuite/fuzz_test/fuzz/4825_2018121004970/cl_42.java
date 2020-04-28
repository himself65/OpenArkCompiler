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

//SEED=2309790164

//import java.util.zip.CRC32;

class cl_42 
{
	   byte var_91 = (byte)(-25);
	   int var_108 = (-836233929);
	   float var_125 = (6.6401e-33F);
	   int [][][] var_151 = {{{(1547868765),(-441036367)},{(-1609604360),(757351172)},{(-1657701628),(-877327111)},{(-222367922),(-1341810560)},{(-1339355081),(1103704780)}},{{(931223025),(-640792912)},{(-1290609025),(-1996882014)},{(2054089252),(390188892)},{(-1013964122),(-822277118)},{(751739597),(657617199)}},{{(1219647746),(-1465065576)},{(-337900745),(702745488)},{(-646852106),(1214943955)},{(-1479676882),(1283888310)},{(-1655954534),(-708427909)}},{{(30980784),(-731228442)},{(302569819),(-2113038906)},{(136484805),(-405228398)},{(1764558422),(791593181)},{(1929297698),(840363477)}},{{(-1780480765),(1679357909)},{(-58413451),(-1870523956)},{(2103247219),(-2081848758)},{(-1400635151),(-864149323)},{(75362637),(176680947)}},{{(532346093),(2056781852)},{(349436307),(810958220)},{(-670932097),(-1581960415)},{(-1442343479),(-834758913)},{(692902025),(293956820)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(249);
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		CrcCheck.ToByteArray(this.var_108,b,"var_108");
		CrcCheck.ToByteArray(this.var_125,b,"var_125");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_151[a0][a1][a2],b,"var_151" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
