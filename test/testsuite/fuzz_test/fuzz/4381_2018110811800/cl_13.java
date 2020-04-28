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

//SEED=4093507686

import java.util.zip.CRC32;

class cl_13 
{
	   int var_43 = (-290735559);
	   float var_47 = (-9.73824e-09F);
	   short var_57 = (short)(-18176);
	final   double var_67 = (-1.1249e+78D);
	   byte var_70 = (byte)(-38);
	   float [][][] var_73 = {{{(-4.03514e+14F),(0.000291683F)},{(9.27866e-19F),(-2.96779e+15F)},{(-1.41534e+07F),(4.67288e-05F)},{(-4.39985e+13F),(-0.00666687F)},{(4.4928e-26F),(-1.8676e-06F)},{(3.32846e+23F),(3091.18F)},{(1.26916e+37F),(2.25058e-19F)}},{{(-6.644e-37F),(-7.04248e-37F)},{(1.20972e-12F),(-2221.5F)},{(1.87276e-17F),(2219.94F)},{(3.66002e-30F),(1.78455e-33F)},{(-8.8663e-28F),(-3.17405e+27F)},{(-1.86676F),(-2.9793e+28F)},{(-2.80894e+15F),(-8.33024e+33F)}}};
	   long var_112 = (-7012547105102853200L);
	   float [][][] var_114 = {{{(-2.60602e-23F),(1.00299e-09F),(-1.20225e-27F)},{(-4.23153e+16F),(-5.60362e+19F),(1.85628e-38F)},{(1.56057e-06F),(-2.57296e+12F),(-2.33438e-08F)},{(-555.283F),(-1.70998e-28F),(2.59107e+19F)},{(8.17428e+18F),(-1.29733e+32F),(-2.70627e-05F)},{(1.03195e-14F),(440270F),(7.71178e+08F)}},{{(2.62065e+36F),(723649F),(7.99891e-19F)},{(-6.48944e+29F),(-3.49422e-26F),(-5.82777e-36F)},{(-6.7104e-26F),(-5.74346e-15F),(-235.332F)},{(-2.04872e-11F),(-6.23566e-38F),(1.02213e+12F)},{(4.50477e+33F),(-5.03836e+31F),(2.73621e-14F)},{(-7.91225e-07F),(-1.28634e-25F),(-3.80469e-14F)}},{{(6.25646e+33F),(-5.74378e-22F),(-130089F)},{(1.08585e-20F),(9.91357e+22F),(-0.00105807F)},{(-4.93684e-23F),(-2.67872e+12F),(-6.44456e+34F)},{(2.18428e-09F),(-3.21431e-13F),(2.88508e+30F)},{(1.51817e-19F),(-1.13403e+17F),(1.16061e-33F)},{(-5.12732e-14F),(9.93017e+24F),(8.18179e+32F)}},{{(-1.87528e+11F),(3.61224e-07F),(2.27451e+07F)},{(4.47369e-14F),(2.54281e-14F),(-1.40084e+09F)},{(0.00561948F),(-1736.5F),(-252.023F)},{(-7.87662e-10F),(-4.13842e+25F),(-8.57525e+29F)},{(-6.03393e-21F),(-1.48243e+35F),(-2.16053e+15F)},{(1.90587e+22F),(-6.24434e+22F),(4.24747e+06F)}},{{(-3.35917e+24F),(-1.26564e-19F),(9.7409e-37F)},{(-9.57681e+08F),(1.94682e+38F),(-9.20547e-30F)},{(37166.2F),(-3.37687e-15F),(-6.54517e+16F)},{(-1.15309e+38F),(4.87242e-18F),(8.97784e-38F)},{(4.65726e-30F),(-1.66612e+21F),(-2.97216e+33F)},{(-43513.7F),(1.80804e-07F),(1.07923e-09F)}},{{(1.4601e+14F),(-2.20572e-10F),(1.19244e-18F)},{(-7.1086e-06F),(-3.55678e+19F),(4.70936e+17F)},{(-1.33763e+25F),(1.67296e+24F),(-736.13F)},{(-4.71928e+13F),(3.69018e-35F),(-3.94903e+34F)},{(1.93817e-19F),(-1.14414e-29F),(1.13885e-08F)},{(-2.38813e+30F),(2.51638e+09F),(8.99075e+08F)}},{{(4.21722e-35F),(8.36757e+30F),(3.72266e+37F)},{(1.26582e+17F),(-9.18496e+31F),(-1.52847e-15F)},{(1.53948e-37F),(2.39115e+37F),(-4.38372e+28F)},{(-2.10941e+08F),(3.04956e+14F),(-7.49563F)},{(-1.97344e+25F),(-9.10306e+14F),(-5.01733e-19F)},{(6.07552e+22F),(-1.87755e-18F),(-3.35299e+21F)}}};
	   boolean var_128 = true;
	   double var_148 = (-9.97571e+187D);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(652);
		CrcCheck.ToByteArray(this.var_43,b,"var_43");
		CrcCheck.ToByteArray(this.var_47,b,"var_47");
		CrcCheck.ToByteArray(this.var_57,b,"var_57");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_73[a0][a1][a2],b,"var_73" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<3;++a2){
			CrcCheck.ToByteArray(this.var_114[a0][a1][a2],b,"var_114" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_128,b,"var_128");
		CrcCheck.ToByteArray(this.var_148,b,"var_148");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
