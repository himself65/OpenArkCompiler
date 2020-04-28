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

//SEED=891913062

//import java.util.zip.CRC32;

class cl_22 
{
	   double var_52 = (-1.58283e-192D);
	   double [][] var_60 = {{(-1.13623e-26D),(1.15295e-95D)}};
	   short var_70 = (short)(-20606);
	   long var_77 = (-9162618765334925357L);
	   boolean var_79 = true;
	   float var_83 = (-0.117283F);
	   int [][] var_85 = {{(1140900208),(-522518081),(1900805229),(1015796998),(-2054220036),(-153393417)},{(395530235),(-1092253506),(1059035801),(665929193),(-1161333663),(677400860)},{(322830173),(-180777288),(-1662218550),(-1791945529),(-1064377892),(-1107136883)},{(73233250),(-1541850910),(1946448617),(979927403),(623189034),(-202989021)},{(924605819),(1237579029),(-1456378024),(1248684531),(1318820020),(953459018)},{(-617757103),(-1756645354),(972588626),(859275119),(297054422),(53630449)},{(1800019048),(493799041),(1190037418),(760063388),(-1696223875),(-1724590965)}};
	   byte var_98 = (byte)(-52);
	   float [][] var_104 = {{(-6.37308e-08F),(9.56346e-24F),(3.08358e-06F)},{(1.76707e-34F),(-3.83178e-32F),(-1.27352e+15F)},{(-4.32066e+24F),(-26.8362F),(-5.65954e-32F)},{(-1.90993e-08F),(1.08427e+26F),(-7.70627e-29F)},{(-1.79774e+10F),(-2.98281e+14F),(-5.8771e-17F)}};
	   int var_105 = (281811251);
	   double [][][] var_228 = {{{(-4.26121e-270D)},{(3.96918e+24D)}}};
	   long [][] var_233 = {{(3919675492134171771L),(7134998223499946416L),(1086193755178154353L),(-7075046952229330319L),(-2224761736025093518L),(-2274217576803292097L),(-3609729031079425131L)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(344);
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_60[a0][a1],b,"var_60" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_77,b,"var_77");
		CrcCheck.ToByteArray(this.var_79,b,"var_79");
		CrcCheck.ToByteArray(this.var_83,b,"var_83");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<6;++a1){
			CrcCheck.ToByteArray(this.var_85[a0][a1],b,"var_85" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_104[a0][a1],b,"var_104" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_105,b,"var_105");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_228[a0][a1][a2],b,"var_228" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_233[a0][a1],b,"var_233" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
