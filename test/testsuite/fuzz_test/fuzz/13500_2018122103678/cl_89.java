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

class cl_89 
{
	   float [][][] var_72 = {{{(-1.21131e+21F),(-1.1053e-16F),(-8.33178e+34F),(2.72419e-39F),(6.11681e+33F)},{(-4.34248e-29F),(4.79482e+17F),(0.657722F),(2.0259e-17F),(1.08924e-23F)},{(-5.64365e-19F),(97.6529F),(-3.95685e+08F),(2.0632e+37F),(-1.72918e+16F)},{(8.84151e+16F),(-2.21704e+17F),(2.20868e-09F),(-1.08082e+28F),(-6.08663e-25F)},{(0.000106128F),(7.47109e+06F),(2.04534e+27F),(-8.6202e-17F),(6.95345e-12F)}},{{(-2.67362e-24F),(-3.48102e+15F),(-2.41147e+33F),(2.2237e+20F),(-1.4197e-30F)},{(-273.208F),(-2.21018e+35F),(-1.07757e+11F),(441.145F),(-3.62731e-05F)},{(-9.64094e-29F),(-2.3644e-23F),(5.3079e+25F),(3.80908e-37F),(-4.44177e-10F)},{(5.82482e-10F),(5.22059e-14F),(1.07939e-19F),(3.60888e+24F),(5.37293e+09F)},{(-2.78678e+26F),(-1.51145e+18F),(9.59852e+07F),(-6.74729e+25F),(1.90681e-07F)}},{{(-3.90686e-31F),(-4.0583e+21F),(-1.42941e-08F),(1.71939e+15F),(6.90912e-36F)},{(-5.07541e-21F),(2.94065e-28F),(-1.66963e+11F),(1.04079e-35F),(-2.27091e+24F)},{(-4.77289e-30F),(2.49279e+38F),(2.18304e-08F),(-7.62711e-35F),(-3.89035e-19F)},{(-2.61011e-08F),(1.62993e+24F),(-9.6026e-30F),(6.84713e-33F),(-4.04752e-06F)},{(1.90869e+18F),(1.68712e-34F),(-0.235529F),(-3.82783F),(1.58274e-18F)}},{{(4.26107e+28F),(-4.21541e-09F),(-1.61221e+06F),(-1.95432e-12F),(-6.29242e+35F)},{(523.095F),(-1.11551e+08F),(-2.74423e-18F),(5.05849e-29F),(6.60128e-14F)},{(-8.68724e+35F),(2.94132e-10F),(2.42557e+13F),(4.40372e+12F),(4.96645e-28F)},{(-0.00049327F),(1.01005e-06F),(-2.12555e+13F),(-2.24657F),(5.18279e-36F)},{(-1.93714e+18F),(0.257501F),(3.8376e+16F),(2846.32F),(-1.57451e+38F)}}};
	final   int var_114 = (1873857947);
	   short var_121 = (short)(553);
	final   long var_190 = (986337916653058172L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(414);
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_72[a0][a1][a2],b,"var_72" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CrcCheck.ToByteArray(this.var_121,b,"var_121");
		CrcCheck.ToByteArray(this.var_190,b,"var_190");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
