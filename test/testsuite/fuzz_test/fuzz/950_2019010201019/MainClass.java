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

//SEED=891913062

//import java.util.zip.CRC32;

class MainClass 
{
	   int var_2 = (2146229479);
	   byte var_6 = (byte)(-65);
	   byte var_7 = (byte)(2);
	   cl_16 var_269 = new cl_16();
	   cl_22 var_271 = new cl_22();
	   byte var_272 = (byte)(-44);
	   byte var_274 = (byte)(89);
/*********************************/
public strictfp void run()
{
	   cl_16 [][] var_5 = {{new cl_16(),new cl_16(),new cl_16(),new cl_16(),new cl_16()},{new cl_16(),new cl_16(),new cl_16(),new cl_16(),new cl_16()}};
	   int var_0 = (-402150138);
	   int var_1 = (899471717);
	   float [][] var_273 = {{(0.000661089F),(-7.57298e+19F),(1.14346e+22F),(1.8068e+15F),(-9.76507e-22F),(17.7746F),(-7.60457e+35F)},{(-0.00717972F),(5.68144e+19F),(-0.000197248F),(3.0041e+16F),(3.43892e-11F),(-1.98188e+18F),(-2.09758e-31F)},{(-2.17942e+37F),(-8.98393e-35F),(6.14997e-19F),(2.33687e+10F),(759.761F),(-2.20643e+26F),(1.78145e-30F)},{(6.07886e-05F),(6.05346e-12F),(-3.10547e-26F),(8.00567F),(1.59597e+29F),(2.54163e+18F),(5.60818e+26F)},{(-13.7298F),(0.00890299F),(-3.28114e+29F),(3.8621e+10F),(4.52111e+14F),(-0.00757979F),(-20111.2F)},{(-2.88573e+38F),(4.37368e-33F),(0.000477598F),(-2.98987e-19F),(727.092F),(6.48941e+19F),(-2.1691e+35F)}};
	for( var_0 = 814 ;(var_0>798);var_0 = (var_0-8) )
	{
		var_1 = (var_1--) ;
		   cl_22 var_270 = new cl_22();
		for( var_2 = 126 ;(var_2>70);var_2 = (var_2-14) )
		{
			   int var_3 = (1253864195);
			for( var_3 = 853 ;(var_3<861);var_3 = (var_3+2) )
			{
				   cl_16 var_276 = new cl_16();
				try
				{
					   cl_16 var_275 = new cl_16();
					var_5[((((var_7--)+var_269.func_8(var_270 ,( ( double )var_271.var_105 ) ,((var_6++)-var_271.var_70) ,var_269.var_112.var_113 ,(~(var_272--)) ,var_273[(((-(var_274--))+var_269.var_173)&5)][((var_1--)&6)]))&var_271.var_98)&1)][((var_6--)&4)] = var_276 ;
				}
				catch( java.lang.ArrayIndexOutOfBoundsException myExp_277 )
				{
					   float [][][] var_278 = {{{(5.4814e+23F),(2.3889e+27F),(-1.61564e-23F),(5.26907e-32F),(1.46714e+38F),(5.48471e-27F)},{(5.65326e-12F),(-5.7253e+07F),(3.77543e-26F),(9.07915e+31F),(28.9343F),(-1.33793e+36F)},{(1.17083e-30F),(2.61508e+26F),(-1.23743e+36F),(-0.00182486F),(-9.44175e-29F),(-6.35386e-16F)},{(-4.57474e+34F),(-5.78743e+24F),(-4.17356e+27F),(1.02571e-17F),(5.83021e-20F),(1.16245e-24F)},{(2.81907e-14F),(-5.56676e+33F),(3.10777e-34F),(-3.51015e+31F),(-8.53833e-13F),(1.90719e+16F)}}};
					System.out.println("hello exception 0 !");
				}
			}
		}
	}
	return ;
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(24);
		CrcCheck.ToByteArray(this.var_2,b,"var_2");
		CrcCheck.ToByteArray(this.var_6,b,"var_6");
		CrcCheck.ToByteArray(this.var_7,b,"var_7");
		CrcCheck.ToByteArray(this.var_269.GetChecksum(),b,"var_269.GetChecksum()");
		CrcCheck.ToByteArray(this.var_271.GetChecksum(),b,"var_271.GetChecksum()");
		CrcCheck.ToByteArray(this.var_272,b,"var_272");
		CrcCheck.ToByteArray(this.var_274,b,"var_274");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
