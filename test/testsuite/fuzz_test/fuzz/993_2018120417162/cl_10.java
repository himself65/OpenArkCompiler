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

//SEED=4271641260

//import java.util.zip.CRC32;

class cl_10 
{
	final   short var_33 = (short)(6274);
	   int var_45 = (-1735764227);
	   byte var_79 = (byte)(-77);
	   boolean var_95 = false;
	   boolean [] var_119 = {true,true,true,false,false};
	final   long var_130 = (-2094143304841338197L);
	   double var_139 = (-5.21257e-183D);
	   long [][][] var_151 = {{{(8656675681041519096L),(6922680441389107082L),(-3851492691654669111L),(3445508424033238979L),(-7040742781601678797L),(-838952237735657361L)},{(1947404278842010262L),(-2928640209538059670L),(-8229470386059936307L),(4959314090259710557L),(8051742362045607756L),(5900647960977003344L)}},{{(6913488162287879620L),(7714279040120213343L),(-1170570321121121678L),(2081301737374067107L),(-5279364575981767907L),(-1775307192308458823L)},{(2423598967838037635L),(10157368915656686L),(1565090104729698574L),(5083802250570986604L),(1660492944374774065L),(273814523805584301L)}},{{(7423302648985163908L),(4907089244064430573L),(-1333650164562375213L),(5938476548469329868L),(3856223701992892588L),(648257843224253086L)},{(6632705538240429770L),(5167495530866021049L),(-7064930790681798901L),(4052131729773672565L),(2435662808104258835L),(3123529464224837752L)}}};
	   long var_170 = (6750630273168338814L);
	   float [][][] var_219 = {{{(3.25378e+13F)},{(6.44254e+28F)},{(-7.08975e+16F)}},{{(-3.66211e+27F)},{(-2.20177e-32F)},{(-1.37486e+31F)}},{{(-4.75734e-07F)},{(1.53391e-10F)},{(2.33162e+16F)}},{{(-8826.61F)},{(4.14285e+32F)},{(-3.36052e+16F)}}};
	   float var_266 = (-1.89387e+38F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(377);
		CrcCheck.ToByteArray(this.var_33,b,"var_33");
		CrcCheck.ToByteArray(this.var_45,b,"var_45");
		CrcCheck.ToByteArray(this.var_79,b,"var_79");
		CrcCheck.ToByteArray(this.var_95,b,"var_95");
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_119[a0],b,"var_119" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_130,b,"var_130");
		CrcCheck.ToByteArray(this.var_139,b,"var_139");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_151[a0][a1][a2],b,"var_151" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_170,b,"var_170");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_219[a0][a1][a2],b,"var_219" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_266,b,"var_266");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
