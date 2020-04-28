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

class cl_8 
{
	   cl_17 var_13 = new cl_17();
	   cl_17 var_15 = new cl_17();
	   int var_19 = (-762497677);
	   byte var_20 = (byte)(49);
	   boolean [][] var_75 = {{false,false,true,true,false}};
	   boolean [] var_78 = {true};
	   float var_81 = (-118.892F);
	   cl_34 var_104 = new cl_34();
/*********************************/
public strictfp byte func_12()
{
	   short var_76 = (short)(31106);
	try
	{
		   byte var_21 = (byte)(89);
		   cl_17 var_16 = new cl_17();
		try
		{
			   float [][][] var_18 = {{{(-1.34507e+36F),(-9.05176e+20F)}},{{(-3.07299e-22F),(1.70958e-23F)}}};
			try
			{
				var_15 = var_16 ;
				var_18[((+(var_21++))&1)][((-(var_20--))&0)][((++var_19)&1)] = var_18[((+(var_20++))&1)][(((var_21--)|var_15.func_22())&0)][1] ;
			}
			catch( java.lang.ArithmeticException myExp_69 )
			{
				var_16 = var_13 ;
				System.out.println("hello exception 5 !");
			}
		}
		catch( java.lang.ArrayIndexOutOfBoundsException myExp_70 )
		{
			final   boolean [] var_71 = {true,false,true,false,false,false,false};
			System.out.println("hello exception 6 !");
		}
	}
	catch( java.lang.ArrayIndexOutOfBoundsException myExp_72 )
	{
		try
		{
			   int var_73 = (-1810976236);
			for( var_73 = 353 ;(var_73>344);var_73 = (var_73-3) )
			{
				var_75[((-((--var_20)&var_15.func_22()))&0)][((var_76--)&4)] = (!((~(var_76--))>=var_15.var_38)) ;
			}
		}
		catch( java.lang.IllegalArgumentException myExp_77 )
		{
			   double [] var_80 = {(-4.91994e+218D)};
			if( false)
			{
				var_80[((++var_20)&0)] = (var_81--) ;
			}
			else
			{
				   byte var_82 = (byte)(24);
				var_76 = (++var_82) ;
			}
			System.out.println("hello exception 8 !");
		}
		System.out.println("hello exception 7 !");
	}
	return (++var_20);
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(39);
		CrcCheck.ToByteArray(this.var_13.GetChecksum(),b,"var_13.GetChecksum()");
		CrcCheck.ToByteArray(this.var_15.GetChecksum(),b,"var_15.GetChecksum()");
		CrcCheck.ToByteArray(this.var_19,b,"var_19");
		CrcCheck.ToByteArray(this.var_20,b,"var_20");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_75[a0][a1],b,"var_75" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_78[a0],b,"var_78" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_104.GetChecksum(),b,"var_104.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
