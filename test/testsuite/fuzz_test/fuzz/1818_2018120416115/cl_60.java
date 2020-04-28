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

//SEED=1973468952

//import java.util.zip.CRC32;

class cl_60 
{
	final   float [] var_69 = {(6.56556e+13F)};
	   short var_87 = (short)(29287);
	   float var_90 = (4.63924e-14F);
	   int var_96 = (-175396577);
	   long var_114 = (787996843223091650L);
	   byte var_164 = (byte)(-38);
	   double var_273 = (-7.74087e-75D);
	   int [][][] var_301 = {{{(-840445731),(-165470277),(-593426325),(2118097335),(-1598953506),(-1120619405)},{(163986892),(-1063722499),(-764328478),(-1989941752),(-1620456981),(-1086448432)},{(-72182520),(1398987908),(-1022508140),(-608135506),(-1179100053),(-290355648)},{(-1462470160),(-291153072),(813579666),(-1629988806),(326192116),(1346721248)},{(-1563509581),(-1654095042),(449593981),(1202355805),(713757549),(1506339646)},{(2040214782),(-609562667),(1339385452),(-324048011),(484450714),(95253296)}},{{(1638161260),(-1838814505),(2119056541),(-1888128048),(1614820163),(775871585)},{(897483322),(1085225290),(1403284868),(-154392639),(2067106833),(1194484854)},{(43829522),(2069875297),(-454899654),(-655509466),(-1449504974),(834716584)},{(-1145938461),(-1862280854),(1802172254),(988279365),(-1066823240),(-2132723628)},{(-507834619),(1338364225),(-707934237),(-811411868),(1260787998),(-1881039537)},{(-1246947765),(1964478395),(1363639832),(597364033),(-1029337323),(77161101)}}};
	   boolean var_313 = true;
	   int [] var_515 = {(-391756383),(1238678474),(-2104454405),(-473766630),(-366932802)};
	final   double [][] var_588 = {{(7.17365e+278D),(-3.66681e-182D),(-4.44883e+243D),(7.12164e+27D),(-2.26454e-132D)},{(-1.82667e-100D),(-2.00706e-156D),(-2.5101e-07D),(-7.73188e+263D),(4.37003e+28D)},{(-1.39556e-294D),(-5.48576e+103D),(-3.02874e+279D),(-1.51551e-269D),(1.0691e+257D)},{(-3.35192e-141D),(-3.71682e+116D),(-4.1723e+284D),(-6.48017e-152D),(-2.27408e-93D)},{(-9.21781e-262D),(-5.45543e-168D),(-1.55648e+91D),(1.85798e-301D),(-7.04485e+224D)},{(-1.22453e-246D),(-1.10773e-237D),(-2.70859e+94D),(-1.63832e+234D),(3.88839e+297D)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(580);
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_69[a0],b,"var_69" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_87,b,"var_87");
		CrcCheck.ToByteArray(this.var_90,b,"var_90");
		CrcCheck.ToByteArray(this.var_96,b,"var_96");
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CrcCheck.ToByteArray(this.var_164,b,"var_164");
		CrcCheck.ToByteArray(this.var_273,b,"var_273");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_301[a0][a1][a2],b,"var_301" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_313,b,"var_313");
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_515[a0],b,"var_515" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_588[a0][a1],b,"var_588" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
