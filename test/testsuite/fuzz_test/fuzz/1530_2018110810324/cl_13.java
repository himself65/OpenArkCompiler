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

//SEED=1354424452

import java.util.zip.CRC32;

class cl_13 
{
	   long var_44 = (-6157810538718583222L);
	   short var_55 = (short)(2176);
	   byte var_56 = (byte)(119);
	   long [][][] var_70 = {{{(-3028691221828248019L),(-3541591075100943849L),(2488708757600134871L),(5413937339581184791L)},{(-7661936276065522613L),(-3105959205337319735L),(1498859235831005585L),(8041448763113512630L)}}};
	   long [][][] var_96 = {{{(-2416517811017384155L),(-4913031168551698853L),(2463149162242510785L)},{(-7799997030830875766L),(6679649143404341122L),(422965013312155034L)}},{{(-6546504353820141772L),(2888659151291569904L),(2363202314755276187L)},{(-6378267911360917038L),(-2039080946833159884L),(-5709440097451578094L)}},{{(-6062084168753824719L),(-6689952799568506289L),(8293461429733646888L)},{(-1000246173342359386L),(-1800027484265911865L),(3893095000980708477L)}},{{(7318455396007708934L),(-6169541502322381178L),(-6187570316670882415L)},{(-6757696411147646379L),(6714597781700736380L),(6087796075427499545L)}}};
	final   int var_133 = (153766925);
	   long [] var_145 = {(-8757912649336820329L),(4356495906309329480L),(-6737811203848919394L),(-8076914960065883557L),(3253041817966579877L)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(311);
		CrcCheck.ToByteArray(this.var_44,b,"var_44");
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_70[a0][a1][a2],b,"var_70" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<3;++a2){
			CrcCheck.ToByteArray(this.var_96[a0][a1][a2],b,"var_96" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_133,b,"var_133");
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_145[a0],b,"var_145" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
