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

//SEED=4207841504

import java.util.zip.CRC32;

class MainClass 
{
	   int var_0 = (-1476218996);
	   byte var_1 = (byte)(-14);
	   float var_2 = (-120380F);
	   cl_10 var_129 = new cl_10();
	final   short var_130 = (short)(17519);
	   short var_132 = (short)(-26979);
	final   cl_30 var_135 = new cl_30();
	   cl_30 var_136 = new cl_30();
	   float [] var_140 = {(-831121F),(-1.73252e-28F),(-1.86107e+26F),(-0.0273218F),(-340.879F),(-4.05017e-12F)};
	   byte var_144 = (byte)(-22);
/*********************************/
public strictfp void run()
{
	   long var_141 = (-7811991078010770292L);
	try
	{
		   cl_10 [][][] var_7 = {{{new cl_10()},{new cl_10()},{new cl_10()},{new cl_10()}},{{new cl_10()},{new cl_10()},{new cl_10()},{new cl_10()}},{{new cl_10()},{new cl_10()},{new cl_10()},{new cl_10()}},{{new cl_10()},{new cl_10()},{new cl_10()},{new cl_10()}},{{new cl_10()},{new cl_10()},{new cl_10()},{new cl_10()}}};
		   float [] var_3 = {(2.74098e+26F),(2.78115e-23F),(-1.12842e-29F),(1.07519e-35F),(7.15578e-31F),(-4.77547e-13F)};
		final   cl_10 var_133 = new cl_10();
		   byte var_138 = (byte)(-31);
		try
		{
			   short var_4 = (short)(-31368);
			   byte var_5 = (byte)(-128);
			try
			{
				   int var_131 = (-323860893);
				var_0 = (var_1++) ;
				var_2 = ((var_5--)-var_2) ;
				var_7[(((++var_132)+( ( short )(+((++var_132)&var_130)) ))&4)][(((((var_5--)|var_129.func_8((++var_5) ,( ( boolean )((++var_4)>=(1916790553)) ) ,(var_1++)))*var_130)-( ( short )((--var_4)|var_131) ))&3)][0] = var_129 ;
			}
			catch( java.lang.IllegalArgumentException myExp_134 )
			{
				var_136 = var_135 ;
				System.out.println("hello exception 0 !");
			}
		}
		catch( java.lang.ArrayIndexOutOfBoundsException myExp_137 )
		{
			if( ((--var_138)>var_1))
			{
				var_140[(((++var_138)*( ( byte )(-(var_138++)) ))&5)] = (+(var_141--)) ;
			}
			System.out.println("hello exception 1 !");
		}
	}
	catch( java.lang.IllegalArgumentException myExp_142 )
	{
		   int var_143 = (718878022);
		for( var_143 = 173 ;(var_143>165);var_143 = (var_143-8) )
		{
			try
			{
				var_141 = (var_144--) ;
			}
			catch( java.lang.ArrayIndexOutOfBoundsException myExp_145 )
			{
				var_129.var_20 = var_136 ;
				System.out.println("hello exception 3 !");
			}
		}
		System.out.println("hello exception 2 !");
	}
	return ;
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(62);
		CrcCheck.ToByteArray(this.var_0,b,"var_0");
		CrcCheck.ToByteArray(this.var_1,b,"var_1");
		CrcCheck.ToByteArray(this.var_2,b,"var_2");
		CrcCheck.ToByteArray(this.var_129.GetChecksum(),b,"var_129.GetChecksum()");
		CrcCheck.ToByteArray(this.var_130,b,"var_130");
		CrcCheck.ToByteArray(this.var_132,b,"var_132");
		CrcCheck.ToByteArray(this.var_135.GetChecksum(),b,"var_135.GetChecksum()");
		CrcCheck.ToByteArray(this.var_136.GetChecksum(),b,"var_136.GetChecksum()");
		for(int a0=0;a0<6;++a0){
			CrcCheck.ToByteArray(this.var_140[a0],b,"var_140" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_144,b,"var_144");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
