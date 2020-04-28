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

class cl_24 extends cl_3
{
	final   cl_10 var_32 = new cl_10();
	   cl_10 var_42 = new cl_10();
	   cl_3 var_53 = new cl_3();
	   cl_3 [][][] var_112 = {{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}},{{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}}};
	   long [][][] var_141 = {{{(3178953589100302344L),(2546318983196462002L),(-6307166307037312813L),(1583917655297486247L)},{(-2974034984432713894L),(8062549681401319698L),(-6642852759138793405L),(-2050721837042794774L)},{(-6834725569843821965L),(550199985529203430L),(-5367712306805330351L),(7510872884648664242L)},{(-8075239765277810410L),(-7563123033602742168L),(-3324233395032923943L),(1579244696839235928L)},{(-2464488880161160012L),(7421327599952376332L),(-13738019981753162L),(4995227661117611678L)},{(-5357391111267768401L),(-3921940880475788589L),(-7881185681748933626L),(-5380486300564146750L)}},{{(2328990767149205056L),(4898383722356366194L),(8656791006027921445L),(-7975289779478785772L)},{(8646965177526460491L),(-8003755972518180281L),(8281503859821622092L),(3456594973706501714L)},{(2220170392753101982L),(6970722986093317266L),(1332655310564469009L),(5427081463289490352L)},{(7663489816101659579L),(1027139940184935513L),(8201964729794897181L),(5786882067177117285L)},{(6992906606422272506L),(-820481117471774209L),(-3551184849758578922L),(-2186882187408301275L)},{(5285004297235875138L),(-199706570068041655L),(5446085394226470853L),(-4772072686450419692L)}}};
	   cl_10 [] var_174 = {new cl_10(),new cl_10()};
	   cl_10 [] var_315 = {new cl_10(),new cl_10(),new cl_10(),new cl_10()};
	final   cl_3 [][] var_343 = {{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3(),new cl_3()}};
	   cl_3 [][][] var_390 = {{{new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3()},{new cl_3(),new cl_3(),new cl_3(),new cl_3()}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(5341);
		CrcCheck.ToByteArray(this.var_29,b,"var_29");
		CrcCheck.ToByteArray(this.var_32.GetChecksum(),b,"var_32.GetChecksum()");
		CrcCheck.ToByteArray(this.var_42.GetChecksum(),b,"var_42.GetChecksum()");
		CrcCheck.ToByteArray(this.var_53.GetChecksum(),b,"var_53.GetChecksum()");
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		CrcCheck.ToByteArray(this.var_60,b,"var_60");
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_104,b,"var_104");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_112[a0][a1][a2].GetChecksum(),b,"var_112" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_117[a0],b,"var_117" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_123[a0],b,"var_123" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_137,b,"var_137");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_141[a0][a1][a2],b,"var_141" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_142[a0][a1][a2],b,"var_142" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<2;++a0){
			CrcCheck.ToByteArray(this.var_174[a0].GetChecksum(),b,"var_174" + "["+ Integer.toString(a0)+"]"+".GetChecksum()");
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_215[a0][a1],b,"var_215" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_281,b,"var_281");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_315[a0].GetChecksum(),b,"var_315" + "["+ Integer.toString(a0)+"]"+".GetChecksum()");
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_319[a0][a1][a2],b,"var_319" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_333[a0][a1][a2],b,"var_333" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<6;++a1){
			CrcCheck.ToByteArray(this.var_343[a0][a1].GetChecksum(),b,"var_343" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_390[a0][a1][a2].GetChecksum(),b,"var_390" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
