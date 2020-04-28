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

//SEED=3943380884

//import java.util.zip.CRC32;

class MainClass 
{
	   int var_0 = (990960553);
	   cl_6 var_1 = new cl_6();
	   int var_4 = (433035202);
	   float var_195 = (-2.99246e-23F);
	   cl_28 var_197 = new cl_28();
	   byte var_204 = (byte)(-86);
	final   cl_80 var_205 = new cl_80();
/*********************************/
public strictfp void run()
{
	final   cl_28 var_208 = new cl_28();
	final   byte [][] var_203 = {{(byte)(106)},{(byte)(-45)},{(byte)(-44)},{(byte)(-110)},{(byte)(35)},{(byte)(35)}};
	   short var_196 = (short)(-18680);
	for( var_0 = 490 ;(var_0>452);var_0 = (var_0-19) )
	{
		   int var_2 = (1244243894);
		for( var_2 = 725 ;(var_2>645);var_2 = (var_2-16) )
		{
			   long var_200 = (-388186498224503690L);
			try
			{
				   byte var_202 = (byte)(-78);
				   int var_3 = (60790528);
				   byte var_201 = (byte)(-72);
				   short var_198 = (short)(-1302);
				for( var_3 = 378 ;(var_3>339);var_3 = (var_3-13) )
				{
					   short var_199 = (short)(-21352);
					var_4 = var_1.func_5((var_195--) ,(-2.41051e-166D) ,((var_196--)-var_197.var_32) ,(var_198++) ,var_1.func_5(((++var_199)*(-7318209345340836280L)) ,(var_200--) ,((--var_201)-( ( byte )((--var_202)|( ( int )var_203[(((var_204--)&var_205.var_93)&5)][(((var_201--)+(1377829721))&0)] )) )) ,(var_204--) ,(var_202--))) ;
				}
			}
			catch( java.lang.IllegalArgumentException myExp_206 )
			{
				try
				{
					var_196 = (var_196--) ;
				}
				catch( java.lang.ArrayIndexOutOfBoundsException myExp_207 )
				{
					var_197 = var_197 ;
					System.out.println("hello exception 1 !");
				}
				System.out.println("hello exception 0 !");
			}
		}
	}
	return ;
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(37);
		CrcCheck.ToByteArray(this.var_0,b,"var_0");
		CrcCheck.ToByteArray(this.var_1.GetChecksum(),b,"var_1.GetChecksum()");
		CrcCheck.ToByteArray(this.var_4,b,"var_4");
		CrcCheck.ToByteArray(this.var_195,b,"var_195");
		CrcCheck.ToByteArray(this.var_197.GetChecksum(),b,"var_197.GetChecksum()");
		CrcCheck.ToByteArray(this.var_204,b,"var_204");
		CrcCheck.ToByteArray(this.var_205.GetChecksum(),b,"var_205.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
