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

//SEED=1217165572

//import java.util.zip.CRC32;

class cl_12 
{
	   byte var_81 = (byte)(-96);
	final   float var_112 = (-4.47051e-33F);
	final   short var_125 = (short)(-5464);
	   float [][][] var_308 = {{{(3.6722e+14F),(-6.18495F),(8.41524e+07F),(1334.27F),(2.51303e-08F),(3.79881e-22F)},{(-3.86865e-28F),(-2.0147e-23F),(5.21183e-23F),(-1.88301e+10F),(-2.58313e-23F),(-1.67041e-10F)},{(7.96215e+15F),(4.70789e-09F),(-1.36946e-08F),(-1.46485e-32F),(36752.1F),(1.60984e+09F)},{(-1.27426e-13F),(5.67981e+36F),(1.36441e+12F),(2.46904e+11F),(-1.09666e+15F),(1.42691e-35F)},{(2.96039e+06F),(8.22745e-12F),(7.95806e-09F),(-7.87262e+19F),(1.10781e+18F),(-2.52176e+21F)},{(1.86561e-20F),(7.22447F),(1.55892e-24F),(-9.64016e-36F),(-1.74954e+15F),(7.05262e+06F)},{(-5.82789e-08F),(4.09471e-22F),(1.80884e+06F),(2.10745e+31F),(1.14064e+17F),(3.14657e-22F)}},{{(-2.69568e+29F),(2.71819e-07F),(-7.47003e+32F),(718754F),(2.90317e-31F),(4.69874e+29F)},{(3.86665e+37F),(-1.40945e+12F),(-3.20678e+25F),(-2.46068e-34F),(22.0961F),(3.16395e+09F)},{(-0.562878F),(1.8739e+33F),(-1126.25F),(1.44505e-37F),(8.06428e+30F),(3.33435e+06F)},{(6.53116e+08F),(-191.693F),(-1.67371e-09F),(3.17236e-29F),(-3.0746e-32F),(-2.661e-07F)},{(9.36328e-25F),(-5.84908e-28F),(2.05513e+17F),(8.53623e-20F),(6.46776e+33F),(1.87798e+10F)},{(6.48342e+27F),(2.42969e+17F),(7.75273e-32F),(2842.8F),(-2.43237e+22F),(-0.0154266F)},{(-3.39226e-26F),(-1.07023e+19F),(5.76376e-16F),(-77.7716F),(7.04736e+35F),(9.07306e+36F)}},{{(3.98573e-24F),(61.4174F),(3.47498e-34F),(-3.84117e+27F),(2.17924e-07F),(3.88433e-10F)},{(-1.23465e+30F),(2.37674e+38F),(496.397F),(1.19103e+17F),(2.98772e-18F),(1.77332e+10F)},{(-1.36047e-24F),(-2.7774e+28F),(-1.46524e-34F),(-9.56228e-25F),(1.25529e+20F),(1.41911e-38F)},{(6.86917e+10F),(1.01343e-21F),(-1165.09F),(-1.9799e-21F),(-0.224028F),(4.40408e-09F)},{(4.78517e-18F),(-4.37797e+11F),(4.70615e-36F),(6.21309e+06F),(-0.0125678F),(2.47927e-24F)},{(8.17864e+33F),(-2.95535e-16F),(1.34025e-35F),(0.0021568F),(3.31259e+10F),(1.20327e+07F)},{(-169.615F),(-3.77726e+14F),(6.07845e+13F),(-6.25974e+37F),(-2.58637e+17F),(3.23537e-18F)}},{{(-7.89233e-10F),(4.40609e-37F),(-1.19836e-31F),(4.59105e-11F),(1.01835e-18F),(-2.65779e-21F)},{(1.17278e-24F),(-6.77956e+29F),(-6.7007e+31F),(-3.74498e+17F),(-1.15749e-26F),(-8.15094e+33F)},{(-1.90932e+11F),(-6.13397e+20F),(-3.97782e-23F),(3.54847e-26F),(-7.65608e+06F),(4.48708e+17F)},{(-9.02522e+27F),(8.62017e+24F),(2.69081e+26F),(-2.57139e+08F),(9.51327e+30F),(2.51017e+18F)},{(2.39746e-06F),(-0.00180289F),(3.3899e+13F),(-973443F),(13404.9F),(-2.38437e-20F)},{(-1.63544e+32F),(-3.52886e+37F),(8.72558e-07F),(4.48712e+15F),(-9.94669e-18F),(-1.00314e-06F)},{(1.17837e+17F),(-3.81529e-20F),(-7.63611e-23F),(2.38602e-18F),(-8.59437e+13F),(6.63706e-33F)}},{{(6.36659e+29F),(8.29746e+35F),(-1.21759e+18F),(-3.52217e-17F),(-1.83207e+21F),(-1.00677e-34F)},{(-1.74542e-22F),(-1.96945e+31F),(1.81541e-18F),(-121540F),(1.02564e+11F),(3.89465e-32F)},{(5.66148e+12F),(6.11592e+22F),(7.23846e-12F),(-1.69148e+31F),(-1.31538e-07F),(-3.29144e-25F)},{(-8.04905e-22F),(-7.58699e-07F),(1.73071e-16F),(1.50238e+25F),(5.89411e+30F),(-3.11306e+10F)},{(-5.27475e+09F),(-1.17944e-30F),(-4.76207e-35F),(-1.09271e-21F),(6.26504e+33F),(2.59276e+35F)},{(4.08629e+34F),(-3.37733e+11F),(1.15683e+36F),(-6.45448e-20F),(-6.76713e+29F),(-5.13435e+13F)},{(4.76246e+06F),(1.06468e-17F),(0.0469628F),(1.37582e-40F),(8.45312e+11F),(1.76688e-26F)}}};
	   byte [][][] var_371 = {{{(byte)(-29)},{(byte)(23)},{(byte)(-114)},{(byte)(-37)},{(byte)(84)},{(byte)(-71)},{(byte)(94)}},{{(byte)(73)},{(byte)(-62)},{(byte)(86)},{(byte)(-33)},{(byte)(-36)},{(byte)(117)},{(byte)(89)}},{{(byte)(-62)},{(byte)(23)},{(byte)(112)},{(byte)(-117)},{(byte)(-103)},{(byte)(-92)},{(byte)(23)}},{{(byte)(108)},{(byte)(6)},{(byte)(25)},{(byte)(31)},{(byte)(-43)},{(byte)(58)},{(byte)(-118)}},{{(byte)(-69)},{(byte)(82)},{(byte)(-14)},{(byte)(101)},{(byte)(-17)},{(byte)(66)},{(byte)(15)}},{{(byte)(-63)},{(byte)(-72)},{(byte)(9)},{(byte)(-37)},{(byte)(-24)},{(byte)(38)},{(byte)(-56)}},{{(byte)(1)},{(byte)(-34)},{(byte)(-84)},{(byte)(17)},{(byte)(-96)},{(byte)(9)},{(byte)(15)}}};
	   boolean var_381 = true;
	   int var_423 = (-1517130329);
	   long var_448 = (5618607935370727832L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(909);
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		CrcCheck.ToByteArray(this.var_125,b,"var_125");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_308[a0][a1][a2],b,"var_308" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_371[a0][a1][a2],b,"var_371" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_381,b,"var_381");
		CrcCheck.ToByteArray(this.var_423,b,"var_423");
		CrcCheck.ToByteArray(this.var_448,b,"var_448");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
