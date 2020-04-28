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

//SEED=4141941138

import java.util.zip.CRC32;

class cl_28 
{
	   boolean var_19 = true;
	   double [][][] var_56 = {{{(-1.63417e+63D)},{(1.52418e-251D)},{(-1.81258e+42D)},{(1.31837e-208D)},{(1.64817e-239D)},{(1.49748e+90D)},{(-4.72892e+69D)}},{{(5.39435e+155D)},{(1.2016e-273D)},{(-1.04007e+11D)},{(-2.4968e-285D)},{(1.30268e+97D)},{(1.40824e-210D)},{(5.61839e+298D)}},{{(-5.46396e+256D)},{(1.39066e+90D)},{(1.6934e+35D)},{(4.32071e+146D)},{(-1.18897e+76D)},{(-3.41942e+164D)},{(-2.51729e-217D)}},{{(3.97199e-201D)},{(-3.3392e+204D)},{(7.17124e-212D)},{(8.51059e-256D)},{(-3.12981e+141D)},{(-3.83934e-167D)},{(-5.9858e-257D)}},{{(2.0348e+144D)},{(-4.94591e+269D)},{(-3.82731e+250D)},{(-1.62349e+105D)},{(1.12306e+133D)},{(2.27424e-160D)},{(-4.21582e-155D)}},{{(1.64864e+124D)},{(-1.02045e-118D)},{(-2.7091e+108D)},{(5.92263e-28D)},{(-6.80011e+82D)},{(-1.4795e+211D)},{(-1.19197e-85D)}},{{(-1.246e-188D)},{(7.16091e+113D)},{(-9.95699e+126D)},{(9.35603e+225D)},{(-1.00886e+22D)},{(3.00308e-44D)},{(-1.23339e+97D)}}};
	final   long var_72 = (1544053462124303389L);
	   short var_80 = (short)(3570);
	   float var_96 = (2.27692e-34F);
	   byte var_114 = (byte)(-114);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(408);
		CrcCheck.ToByteArray(this.var_19,b,"var_19");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_56[a0][a1][a2],b,"var_56" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_72,b,"var_72");
		CrcCheck.ToByteArray(this.var_80,b,"var_80");
		CrcCheck.ToByteArray(this.var_96,b,"var_96");
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
