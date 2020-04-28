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

//SEED=1157171100

//import java.util.zip.CRC32;

class cl_24 
{
	   double var_23 = (-4.8081e+27D);
	   short var_58 = (short)(31410);
	   float [][][] var_69 = {{{(-1.01641e+16F),(4.74462e+33F),(-184160F),(-1.12141e-13F),(1.45964e+29F)},{(1.30558e+08F),(9.2737e-07F),(-1.97102e-20F),(9.72139e-27F),(3.71266e-33F)},{(-7.22072e-27F),(7.03026e-32F),(1.07234e-35F),(3.36238e-26F),(-1.34828e+16F)},{(-1.71536e-06F),(1.402e-36F),(-1.28387e-37F),(-2.0977e+31F),(-1.54607e-22F)},{(-8.65851e+15F),(-1.60956e+19F),(1.21752e+10F),(1.10663e+22F),(-1.84697e-19F)}},{{(-7.74933e+14F),(167192F),(-2.32312e+28F),(-2.34593e-36F),(2.48559e+33F)},{(0.0940007F),(5.7378e-38F),(-3.61502e-14F),(6.33505e-26F),(-5.44983e+20F)},{(-4.97601e-14F),(-3.17273e+10F),(7.13999e-17F),(1.32273e-39F),(-169.542F)},{(-0.249679F),(-1.57294e+13F),(2.62441e-17F),(-4.76363e-06F),(-1.3352e-12F)},{(8.46637e-38F),(-0.000889981F),(5.74302e+26F),(3.96511e+07F),(1.5938e+08F)}},{{(-1.31899e-35F),(-6.87014e+06F),(5.96425e-27F),(-5.32237e-27F),(-1.05479e-18F)},{(-3.40675e-05F),(-1.05976e-18F),(2.87118F),(-292101F),(-1.04585e+20F)},{(-1.08788F),(-3.89923e-30F),(6.06387e-15F),(-1.0092e-16F),(-2.76918e+10F)},{(6.35615e-18F),(-1.52014e+25F),(-623.529F),(9.33616e-10F),(28500.7F)},{(1.31432e+36F),(1.035e+27F),(-8.31763e+36F),(-55400F),(1.3751e+06F)}},{{(-8.53826e-28F),(1.0667e-35F),(5.80768e-20F),(-2.5933e+33F),(3.32962e-12F)},{(-5.27396e-23F),(2.8635e+11F),(3.65366e+29F),(-3.79147e+30F),(1.61e+34F)},{(-2.22105e-07F),(-7.62765e+34F),(-1.16184e-18F),(-4.07976e-17F),(6.92859e+18F)},{(8.43451e+32F),(-2.49584e-30F),(-1.00381e-20F),(-5.62545e+28F),(-2.48891e+36F)},{(1.726e-10F),(5.40144e+22F),(8.61396e+35F),(2.10514e+15F),(-2.19209e-30F)}}};
	   int var_109 = (-1696644156);
	   double [] var_117 = {(1.01381e+261D)};
	final   byte var_122 = (byte)(61);
	   float var_123 = (-1.993e+10F);
	   boolean [][][] var_234 = {{{true,true,true,false,true,true,false},{true,false,false,false,false,true,false},{true,false,true,false,true,true,false},{true,false,false,true,true,false,true},{true,true,true,true,false,false,false},{true,false,false,false,true,false,true},{false,true,true,true,false,false,false}},{{false,false,false,true,true,false,false},{false,true,false,true,false,false,false},{false,false,true,true,false,true,true},{true,false,false,true,false,false,true},{false,true,false,true,true,true,true},{false,true,false,false,true,true,true},{false,false,false,false,false,false,false}},{{true,true,false,true,true,false,true},{false,false,true,true,true,false,true},{true,true,true,false,false,true,false},{true,false,true,true,false,false,false},{false,true,true,true,true,false,true},{false,true,true,false,false,true,false},{false,true,true,false,false,true,true}},{{true,true,true,false,false,false,true},{true,true,false,true,false,false,false},{false,true,true,false,true,false,false},{false,true,true,true,false,true,true},{true,false,true,true,false,true,false},{true,false,true,true,true,true,true},{false,false,false,false,true,true,true}},{{true,true,false,true,false,true,false},{true,true,false,true,false,false,true},{true,true,true,false,true,true,false},{true,false,true,true,true,true,true},{false,false,false,true,false,false,true},{false,true,true,true,false,true,true},{false,true,false,false,false,false,false}},{{false,true,false,true,false,true,true},{false,true,false,false,true,true,false},{true,true,true,false,false,true,true},{true,false,true,false,true,true,false},{false,false,false,false,true,false,false},{false,false,true,false,true,true,true},{false,false,true,true,false,false,false}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(721);
		CrcCheck.ToByteArray(this.var_23,b,"var_23");
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_69[a0][a1][a2],b,"var_69" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_117[a0],b,"var_117" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_122,b,"var_122");
		CrcCheck.ToByteArray(this.var_123,b,"var_123");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_234[a0][a1][a2],b,"var_234" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
