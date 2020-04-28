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

//SEED=205973598

//import java.util.zip.CRC32;

class cl_56 
{
	   int [][][] var_73 = {{{(1961336781)},{(-1709181994)},{(-1767121385)}},{{(1524299597)},{(-335028499)},{(1484784568)}},{{(-683195338)},{(1577601614)},{(-253506153)}},{{(1954894057)},{(1558151970)},{(-583795305)}},{{(285210962)},{(463886050)},{(907299031)}}};
	   float var_96 = (5.55706F);
	final   byte var_116 = (byte)(90);
	   double var_133 = (9.86053e+76D);
	final   int var_226 = (-972001571);
	   long var_260 = (-8753070466079353371L);
	   float [][][] var_271 = {{{(12.3949F),(-1.44143e+35F)},{(-1.6336e-21F),(2.47716e+36F)},{(-6.01359e+32F),(2.1974e+15F)},{(2.89359e-11F),(3.99632e+22F)},{(-1.12805e-15F),(-4.12761e-19F)}},{{(-5.64436e-38F),(29.9785F)},{(-1.89625F),(-6.48737e+32F)},{(-1.64082e-07F),(1.93096e+15F)},{(-3.9671e+12F),(-5.90804e+35F)},{(1.38579e-35F),(3.00651e+29F)}},{{(-1.06887e+07F),(5.11453e+29F)},{(1.05895e-31F),(-1.12325e-33F)},{(8.67038e-13F),(-50686F)},{(-1.92829e+22F),(0.0112868F)},{(7.21592e+21F),(-1.41579e-05F)}},{{(1.15998e-35F),(126583F)},{(-1014.68F),(-6681.22F)},{(-1.17678e-16F),(5.34559e-36F)},{(3.13449e+21F),(-2.10189e-06F)},{(6.20066e+10F),(-1.38597e+18F)}},{{(-2.54722e-23F),(-3.136e-16F)},{(-2.52167e-06F),(3.75221e-37F)},{(2.09495e+23F),(4.44148e+36F)},{(-8.01549e+21F),(679.456F)},{(-5.42828e-14F),(5.04539e+30F)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(285);
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_73[a0][a1][a2],b,"var_73" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_96,b,"var_96");
		CrcCheck.ToByteArray(this.var_116,b,"var_116");
		CrcCheck.ToByteArray(this.var_133,b,"var_133");
		CrcCheck.ToByteArray(this.var_226,b,"var_226");
		CrcCheck.ToByteArray(this.var_260,b,"var_260");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_271[a0][a1][a2],b,"var_271" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
