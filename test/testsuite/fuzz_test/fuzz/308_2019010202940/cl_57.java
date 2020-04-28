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

//SEED=2776528782

//import java.util.zip.CRC32;

class cl_57 
{
	   int [][] var_75 = {{(531397324),(-1250763649),(873139265)},{(1548468637),(-77667901),(-1059722600)},{(1948801826),(-683152146),(880051313)},{(-592523384),(1324162026),(-1405776732)},{(696312779),(-2069455898),(-1329907880)},{(838872338),(-1407850260),(-826154914)},{(2043881257),(-1153561296),(1903623225)}};
	   int var_84 = (-819969934);
	   boolean var_91 = true;
	   double [][][] var_93 = {{{(-1.17446e-163D),(-8.66185e-183D),(-4.82415e-51D),(-2.40048e+256D),(1.00139e-133D)},{(-3.97828e-157D),(-2.76586e-294D),(-4.04337e-177D),(-5.57528e+214D),(-8.36866e+111D)},{(-1.12023e-187D),(5.58702e-30D),(-3.73068e+188D),(5.97791e+146D),(6.18138e+87D)},{(-2.91045e+80D),(-4.26337e+296D),(2.75991e-87D),(-9.56955e+34D),(-1.71918e-56D)}},{{(-8.13778e+28D),(1.48368e-119D),(7.45695e-138D),(1.34076e+100D),(4.2456e-188D)},{(9.43745e+205D),(-4.14638e+12D),(6.36298e+80D),(4.93795e-94D),(-1.35482e+231D)},{(-4.18128e-273D),(4.60284e+160D),(6.46519e-75D),(-1.90988e-06D),(8.59711e+187D)},{(1.43483e+210D),(-3.93167e-77D),(-2.88974e-14D),(2.68972e+271D),(-6.21929e+44D)}},{{(4.61598e-76D),(5.44618e-167D),(1.21001e+79D),(1.43425e-25D),(-8.89474e-117D)},{(3.36615e+97D),(-4.9297e+172D),(-2.30694e-282D),(-3.17598e+104D),(-3.60813e+108D)},{(-1.41939e+212D),(7.37231e-169D),(-2.20638e+279D),(1.76727e+09D),(-7.25437e+24D)},{(9.32765e+285D),(2.3669e+251D),(-1.02651e-137D),(1.07132e-166D),(-1.0214e-226D)}},{{(-1.56431e-201D),(-0.0899253D),(1.34822e+125D),(-8.49477e-296D),(7.50311e-178D)},{(2.14982e+246D),(-1.94892e-64D),(4.69775e-271D),(-2.27502e+43D),(2.10779e+229D)},{(-6.77594e-298D),(-1.96288e-210D),(9.95737e+238D),(-1.08387e-184D),(9.36909e-211D)},{(-4.46416e-276D),(5.53067e-31D),(-9.25546e-120D),(4.56521e-285D),(7.54225e+40D)}},{{(1.24546e+237D),(3.3704e+85D),(1.82643e+254D),(4.10683e+218D),(3.5611e+114D)},{(8.60181e-100D),(4.52641e+258D),(-6.47589e+53D),(-4.67688e-167D),(-2.47371e+182D)},{(3.82207e+83D),(9.83496e-219D),(-1.18319e-62D),(-2.63756e+132D),(-1.62652e+273D)},{(8.63304e+156D),(-2.84256e-178D),(-4.48835e+13D),(-9.51165e+71D),(-1.65108e+88D)}},{{(1.08586e-163D),(8.27014e-228D),(2.96243e-188D),(9.42918e+286D),(1.0422e+173D)},{(4.75131e-195D),(8.40344e-68D),(-1.26318e-191D),(-2.93058e-204D),(5.41014e-201D)},{(1.9492e+94D),(-5.8989e+279D),(1.19992e-181D),(-1.85496e-150D),(1.15236e+226D)},{(-1.97516e-109D),(4.19787e+197D),(-4.93175e-124D),(2.22353e+28D),(-1.02349e-262D)}},{{(-2.38248e+147D),(2.59539e+185D),(-4.10317e+173D),(7.90319e+107D),(2.3722e+07D)},{(8.77518e-188D),(-3.23886e-25D),(-1.4388e-53D),(-1.10977e+14D),(5.54929e+243D)},{(2.4389e-67D),(-7.68957e-129D),(-4.36624e+12D),(-1.28165e-97D),(-4.58848e+265D)},{(4.78146e+271D),(-2.28871e-113D),(-7.37855e-206D),(-5.08298e+180D),(-1.26466e-68D)}}};
	   double var_103 = (-4.61663e-245D);
	   long var_110 = (-3452364138805768001L);
	   short var_112 = (short)(20055);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(1227);
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_75[a0][a1],b,"var_75" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_84,b,"var_84");
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_93[a0][a1][a2],b,"var_93" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_110,b,"var_110");
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
