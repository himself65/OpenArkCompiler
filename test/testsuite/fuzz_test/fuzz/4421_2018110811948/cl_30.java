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

//SEED=1738597656

import java.util.zip.CRC32;

class cl_30 
{
	   boolean var_39 = true;
	   int var_40 = (1611951211);
	   byte var_46 = (byte)(-43);
	   short var_52 = (short)(-21298);
	   double var_54 = (1.17889e-297D);
	   int [] var_57 = {(-536661324),(16343643),(774393382)};
	   long var_64 = (9102588774310969404L);
	   long [][][] var_77 = {{{(-2511102005351033354L),(-978630090420447661L),(-4835063643337143434L),(4539791564346190241L)},{(4587528220981470945L),(-7129192313491234680L),(-528161857177647632L),(-7120100485348189314L)},{(6252516301832085586L),(7043391239657081674L),(-8034406595513134879L),(3513224825781810823L)}},{{(-6010607985777437594L),(-4648621699761415227L),(-5585039729022962044L),(-664359733431122048L)},{(-5313496878532615672L),(-5860856650313023849L),(-6248661515363813871L),(5003638183880687371L)},{(-3726307126320979336L),(-4502859966278208920L),(3208398760264947760L),(-8472873344887757263L)}},{{(-3925116976723419554L),(-1768847774034075776L),(6612907659321557025L),(-1277983811552706531L)},{(-3305416212927268484L),(5097727925215889303L),(-8927743154941866104L),(-1852819001156843639L)},{(-4862556068649593629L),(1907830048821406376L),(-7164873056100358260L),(2847865267445213801L)}}};
	   float var_86 = (0.00387697F);
	final   long [][] var_98 = {{(5798891899726577186L),(3599580295138384362L),(-3912431811196932416L)},{(4151741990219093065L),(-749407215685465658L),(6891471295107466575L)},{(-1747833879490389438L),(7542390628617238346L),(8473175571192116148L)},{(6255311629947454803L),(4746834303895384232L),(8240922169835955018L)},{(1811041125746399313L),(8447725690162246286L),(8334461830759026632L)},{(1863821257795554162L),(3843147083985724345L),(-6844815724691273592L)},{(7590696162917067770L),(5749852070519049863L),(-4891728130826015188L)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(496);
		CrcCheck.ToByteArray(this.var_39,b,"var_39");
		CrcCheck.ToByteArray(this.var_40,b,"var_40");
		CrcCheck.ToByteArray(this.var_46,b,"var_46");
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		CrcCheck.ToByteArray(this.var_54,b,"var_54");
		for(int a0=0;a0<3;++a0){
			CrcCheck.ToByteArray(this.var_57[a0],b,"var_57" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_64,b,"var_64");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_77[a0][a1][a2],b,"var_77" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_86,b,"var_86");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_98[a0][a1],b,"var_98" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
