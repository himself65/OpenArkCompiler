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

//SEED=1808069720

//import java.util.zip.CRC32;

class cl_121 
{
	   int var_85 = (2132076913);
	   int [][][] var_99 = {{{(-1884668886),(-303725599)},{(-1952841514),(-1289161822)},{(648246785),(-1546754193)},{(-1466988075),(-594429864)},{(-1889105944),(1300715815)}},{{(-1872481124),(-561838852)},{(1373314073),(1814163331)},{(-557956060),(451377002)},{(-20650041),(-1222388300)},{(1795812872),(1073559491)}},{{(1805649623),(-1149857861)},{(1733631958),(-1079882836)},{(1727587099),(-378223084)},{(1827595891),(81205332)},{(1113912584),(-1894133015)}},{{(-1716691222),(176708425)},{(-103446015),(1046872222)},{(456662045),(-1058184883)},{(-1966769418),(735571243)},{(513627801),(-2061609666)}}};
	   byte var_104 = (byte)(31);
	   float [] var_112 = {(-28.787F),(-1.03931e-31F)};
	final   double var_143 = (1.09182e-229D);
	   short var_171 = (short)(-16571);
	   long var_380 = (-6257228614868186694L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(191);
		CrcCheck.ToByteArray(this.var_85,b,"var_85");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_99[a0][a1][a2],b,"var_99" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_104,b,"var_104");
		for(int a0=0;a0<2;++a0){
			CrcCheck.ToByteArray(this.var_112[a0],b,"var_112" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_143,b,"var_143");
		CrcCheck.ToByteArray(this.var_171,b,"var_171");
		CrcCheck.ToByteArray(this.var_380,b,"var_380");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
