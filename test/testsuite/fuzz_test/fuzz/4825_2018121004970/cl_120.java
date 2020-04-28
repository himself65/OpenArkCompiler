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

class cl_120 
{
	   short var_87 = (short)(-30307);
	final   double [] var_98 = {(-1.47532e+168D),(-1.53651e+306D),(1.14433e+261D),(4.13971e-202D),(3.02433e+118D),(-1.68545e+72D),(-1.40414e-124D)};
	   float var_101 = (-1.83945e+37F);
	final   int [][][] var_149 = {{{(-1246117603),(-1080757889)},{(-610590317),(1218002460)},{(-2089884237),(2109702998)},{(1218917967),(-418851303)},{(-1122621027),(-660069785)}},{{(-822395502),(640886798)},{(2036320404),(326749496)},{(-1533531329),(-893219280)},{(525354729),(-1265610679)},{(1579406884),(1088646474)}},{{(-1213143560),(-2059033758)},{(-702767798),(1658433867)},{(1475088967),(447618954)},{(1993160463),(750617030)},{(1487662961),(263975422)}},{{(82961734),(-374136109)},{(235660212),(-634983350)},{(-690642596),(603993841)},{(-105577975),(-288315963)},{(1535913995),(-769953035)}},{{(-1364988553),(412723975)},{(-1259038953),(-353301166)},{(1920864433),(-1391871145)},{(1100884313),(-1365197405)},{(-95985627),(1493629554)}},{{(68956879),(1498622969)},{(535361539),(1261533468)},{(1757502222),(940822658)},{(-89302995),(1901237289)},{(-2010303146),(1637042635)}}};
	   long var_165 = (-4030719856334671451L);
	   int var_185 = (-415303754);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(314);
		CrcCheck.ToByteArray(this.var_87,b,"var_87");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_98[a0],b,"var_98" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_101,b,"var_101");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_149[a0][a1][a2],b,"var_149" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_165,b,"var_165");
		CrcCheck.ToByteArray(this.var_185,b,"var_185");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
