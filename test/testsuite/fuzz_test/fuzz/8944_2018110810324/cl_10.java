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

//SEED=2002426344

import java.util.zip.CRC32;

class cl_10 
{
	   long var_34 = (2768219394864161365L);
	   int var_42 = (685153109);
	   boolean var_46 = true;
	   float var_52 = (-4038.78F);
	final   short var_85 = (short)(13491);
	   double var_87 = (-6.23532e+202D);
	   float [][][] var_106 = {{{(6.97381e+13F),(1.46382e-23F),(1.00329e+37F),(0.000108035F),(1.59997e-33F)},{(4.7029e+26F),(-6.4641e+19F),(1.43542e-37F),(0.311861F),(-8.88366e-07F)},{(1.33507e-31F),(-3.13803e-37F),(1.93645e+36F),(1.41674e-34F),(2.68865e-19F)},{(29.5316F),(-16.0588F),(-1.80835e+15F),(1.36302e-12F),(-1.06439e+06F)},{(0.406416F),(-6.77269e+36F),(1.10315e-35F),(2.75015e-36F),(2.18242e+15F)},{(-6.52303e+15F),(-4.06075e-06F),(-9.98246e+14F),(-2.27419e-20F),(2.33271e-31F)},{(-1.70146e-13F),(1.60371e-17F),(-2.30829e+32F),(-1.27981e+14F),(4.62482e-35F)}},{{(-7.80563F),(3.07024e+18F),(-241.709F),(1.24653e-32F),(-5.14992e-18F)},{(-2.8307e-24F),(3.76612e+32F),(7.75555e-20F),(2.42234e+27F),(-2.98765e-22F)},{(-3.1835e+09F),(-0.000121631F),(391.854F),(4.28112e-10F),(-9.85535e+16F)},{(2.23645e+20F),(0.93546F),(-8.06951e+28F),(-4.12815e+24F),(-1.60942e-33F)},{(-8.93193e-37F),(-1.08249e-21F),(1.86596e+26F),(1.09918e+27F),(1.4722e-15F)},{(1677.81F),(-4.27381e-37F),(3.77992e-11F),(-3.73502e+15F),(-3.98109e+14F)},{(3.66725e+20F),(4.99423e-27F),(-1.14326e-11F),(-1.30115e-06F),(7.14156e+07F)}},{{(2.14927e+11F),(1.09572e+31F),(0.00380762F),(2.34444e-18F),(1.09949e+37F)},{(1.676e-23F),(-5.84408e-27F),(-0.0633172F),(6.91108e-37F),(0.00249128F)},{(-3.15531e+13F),(5.34266e+37F),(2.70704e+27F),(-1.2216e+09F),(4.42402e-25F)},{(1.35437e+10F),(5.03317e+15F),(3.01424e-33F),(-3.59152e-06F),(-4.25559e+22F)},{(-1.49647e+07F),(7.76757e-26F),(2.26376e-07F),(4.84116e+08F),(-1.35703e-05F)},{(-1.39455e-38F),(-4.62784e-16F),(4.85102e-08F),(-8.07823e-29F),(-1.22982e-37F)},{(7.03776e-23F),(2.844e-06F),(-2.54869e+10F),(-2.25162e-07F),(656523F)}},{{(1.88992e+10F),(-1.81938e+31F),(-2.40171e-22F),(4.64624e+14F),(-1.57732e+17F)},{(2.28713e-34F),(2.41515e-33F),(2.39198e+27F),(2.04282e-37F),(1.87632e-10F)},{(-1.86083e-38F),(0.00281926F),(1.16883e+33F),(-4.1733e+22F),(-1.63595e+33F)},{(-4.65272e+10F),(3.0113e-28F),(-1.41037e+10F),(-3.21504e+20F),(-4.44763e-31F)},{(-2.58449e+22F),(-3.19765e+13F),(-1.31052e+29F),(-9.37062e-25F),(-9.69698e-08F)},{(-2.96439e+29F),(-7.85585e+21F),(-3.58504e+25F),(-1.9789e+29F),(-1.03382e-10F)},{(6.33382e+32F),(-2.18153e-07F),(-1.40454e-09F),(-9.65384e+08F),(-0.000427121F)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(587);
		CrcCheck.ToByteArray(this.var_34,b,"var_34");
		CrcCheck.ToByteArray(this.var_42,b,"var_42");
		CrcCheck.ToByteArray(this.var_46,b,"var_46");
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		CrcCheck.ToByteArray(this.var_85,b,"var_85");
		CrcCheck.ToByteArray(this.var_87,b,"var_87");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<5;++a2){
			CrcCheck.ToByteArray(this.var_106[a0][a1][a2],b,"var_106" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
