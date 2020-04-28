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

//SEED=2069637170

import java.util.zip.CRC32;

class MainClass 
{
	final   boolean var_0 = true;
	   boolean [] var_2 = {false,false};
	   cl_8 var_83 = new cl_8();
	   byte var_85 = (byte)(-34);
	   cl_17 var_86 = new cl_17();
	   cl_8 var_99 = new cl_8();
	final   cl_34 var_107 = new cl_34();
	   cl_8 var_109 = new cl_8();
	   double [][][] var_111 = {{{(2.68324e-211D),(-1.63381e-206D)},{(3.06936e+209D),(1.02522e-203D)}},{{(-1.38889e+84D),(2.08644e+270D)},{(2.25298e+282D),(2.52566e-142D)}},{{(2.05177e-294D),(-3.14395e+11D)},{(5.54954e+259D),(-2.40273e-100D)}},{{(-2.15281e+66D),(-9.91158e+266D)},{(5.49126e-90D),(1.40017e-102D)}}};
	   int var_114 = (-2096137368);
	   int var_120 = (389382918);
	   float var_121 = (-2.24347e-05F);
	final   short var_122 = (short)(16632);
/*********************************/
public strictfp void run()
{
	   byte var_106 = (byte)(37);
	try
	{
		   short var_3 = (short)(18516);
		try
		{
			   long var_94 = (-1809437483882687760L);
			try
			{
				   cl_8 var_89 = new cl_8();
				   long var_92 = (4736856454249014806L);
				var_2[(((--var_3)+func_4(var_86.var_25.var_87 ,var_89 ,(-7.84389e+234D) ,var_92 ,(++var_94)))&1)] = var_2[((~(++var_85))&1)] ;
			}
			catch( java.lang.IllegalArgumentException myExp_96 )
			{
				   long var_97 = (-5628341308990273530L);
				System.out.println("hello exception 0 !");
			}
		}
		catch( java.lang.ArithmeticException myExp_98 )
		{
			var_83 = var_99 ;
			System.out.println("hello exception 1 !");
		}
	}
	catch( java.lang.ArithmeticException myExp_100 )
	{
		   int var_103 = (651145383);
		try
		{
			   short var_116 = (short)(9690);
			try
			{
				   int [][][] var_102 = {{{(-1173319625)}},{{(-1808601050)}},{{(2061853429)}}};
				var_102[(((var_106--)+func_4(var_107.var_87 ,var_109 ,var_111[((var_103--)&3)][((var_106++)&1)][(((var_85--)+var_85)&1)] ,(+(var_85++)) ,(var_114++)))&2)][((+(var_106--))&0)][((((((var_103++)-(short)(8645))|(byte)(-86))+var_86.var_38)|var_83.var_104.var_105)&0)] = (--var_116) ;
			}
			catch( java.lang.ArrayIndexOutOfBoundsException myExp_117 )
			{
				   long var_118 = (-4429543518404253823L);
				System.out.println("hello exception 3 !");
			}
		}
		catch( java.lang.ArrayIndexOutOfBoundsException myExp_119 )
		{
			for( var_120 = 259 ;(var_120>164);var_120 = (var_120-19) )
			{
				var_111[((++var_85)&3)][((var_85++)&1)][(((var_114--)|var_122)&1)] = ( ( double )(--var_103) ) ;
			}
			System.out.println("hello exception 4 !");
		}
		System.out.println("hello exception 2 !");
	}
	return ;
}
public strictfp short func_4(double[][] var_5, cl_8 var_6, double var_7, long var_8, long var_9)
{
	   byte var_11 = (byte)(1);
	   float var_84 = (0.000877744F);
	   byte var_10 = (byte)(-128);
	var_5[(((--var_11)|var_83.func_12())&5)][((var_10--)&0)] = (var_84--) ;
	return (--var_85);
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(186);
		CrcCheck.ToByteArray(this.var_0,b,"var_0");
		for(int a0=0;a0<2;++a0){
			CrcCheck.ToByteArray(this.var_2[a0],b,"var_2" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_83.GetChecksum(),b,"var_83.GetChecksum()");
		CrcCheck.ToByteArray(this.var_85,b,"var_85");
		CrcCheck.ToByteArray(this.var_86.GetChecksum(),b,"var_86.GetChecksum()");
		CrcCheck.ToByteArray(this.var_99.GetChecksum(),b,"var_99.GetChecksum()");
		CrcCheck.ToByteArray(this.var_107.GetChecksum(),b,"var_107.GetChecksum()");
		CrcCheck.ToByteArray(this.var_109.GetChecksum(),b,"var_109.GetChecksum()");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_111[a0][a1][a2],b,"var_111" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CrcCheck.ToByteArray(this.var_120,b,"var_120");
		CrcCheck.ToByteArray(this.var_121,b,"var_121");
		CrcCheck.ToByteArray(this.var_122,b,"var_122");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
