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

//SEED=764790408

//import java.util.zip.CRC32;

class cl_27 
{
	   cl_37 var_33 = new cl_37();
	   cl_45 var_150 = new cl_45();
	   cl_65 var_151 = new cl_65();
	   cl_96 var_155 = new cl_96();
	   byte var_156 = (byte)(92);
	   short var_157 = (short)(28012);
	   int var_177 = (402882666);
	final   cl_144 var_197 = new cl_144();
	   double var_270 = (1.29591e+197D);
	   cl_144 var_283 = new cl_144();
/*********************************/
public strictfp boolean func_27(cl_37 var_28, float var_29, int var_30, float var_31)
{
	   byte var_154 = (byte)(-97);
	final   cl_96 var_167 = new cl_96();
	   byte var_34 = (byte)(29);
	   int var_32 = (61685064);
	for( var_32 = 497 ;(var_32>481);var_32 = (var_32-16) )
	{
		   byte var_35 = (byte)(118);
		var_33 = var_33 ;
		   int [][][] var_40 = {{{(-1537784933),(-1096184569),(-130534131),(-1877122500),(128617256),(23419590),(480748017)},{(356799673),(-871791058),(-1499058249),(433969350),(-802642297),(-866506263),(-1282252236)},{(-1930207003),(1081229673),(713403077),(-1378762634),(1493671523),(1383366540),(1490529399)}},{{(1814952551),(319087539),(2008513943),(-1643368748),(59869938),(527269440),(-1051473426)},{(-177861564),(750996147),(35203932),(1809657566),(-1767676933),(-109338395),(-824433151)},{(77518001),(1252094944),(-97697702),(1283967264),(1314277886),(-2138171115),(1442446679)}},{{(2136110912),(698313855),(2140944236),(1496709705),(-1763311282),(-1097277555),(1810315848)},{(1825582343),(214047380),(-369998115),(595151981),(1420955513),(-1339869309),(-144820487)},{(-434478593),(-1199368685),(1304836821),(-657560261),(-1878929044),(-689550232),(1078053642)}},{{(1087469184),(-620048632),(-199777154),(-453226691),(-1310281233),(-1946587166),(-263849341)},{(-2096474350),(-2086204056),(-1580097352),(1646039095),(-1964162344),(-884854314),(-1889664048)},{(820582683),(343016359),(-1270817720),(-884210478),(1654751056),(-377869697),(379622900)}}};
		   int [] var_39 = {(-1169296695),(517861080),(1303796327),(959591943),(-1767292750),(450135207)};
		try
		{
			   boolean var_161 = true;
			if( ((--var_34)<=( ( float )(--var_35) )))
			{
			System.out.println(" if stmt begin,id=13 ");
				   long var_152 = (-6500553287566730464L);
				try
				{
					   boolean [][][] var_36 = {{{false,false},{false,false}},{{false,true},{false,false}},{{false,true},{false,true}},{{true,false},{true,false}}};
					if( var_33.var_37)
					{
					System.out.println(" if stmt begin,id=14 ");
						   short var_41 = (short)(-15700);
						   int var_42 = (2370969);
						   int var_38 = (1039676273);
						for( var_38 = 793 ;(var_38>745);var_38 = (var_38-12) )
						{
							var_39[(var_38&5)] = var_40[(((var_42++)|var_150.func_43(var_151 ,((var_152++)*var_33.var_153) ,((+(var_154++))<var_155.var_100) ,(var_152++) ,((var_152--)-var_151.var_75) ,(~((var_35--)|var_150.var_81)) ,((var_156--)-var_157)))&3)][2][((++var_41)&6)] ;
						}
					System.out.println(" if stmt end,id=14 ");
					}
				}
				catch( java.lang.ArrayIndexOutOfBoundsException myExp_158 )
				{
					if( ((var_154++)>( ( float )var_150.func_53((!(!(!((--var_154)<( ( short )(((var_154--)+( ( short )(var_156--) ))&var_33.var_104) )))))) )))
					{
					System.out.println(" if stmt begin,id=15 ");
						try
						{
							var_34 = (++var_35) ;
						}
						catch( java.lang.ArithmeticException myExp_159 )
						{
							var_150.var_67.var_160 = false ;
							System.out.println("hello exception 38 !");
						}
					System.out.println(" if stmt end,id=15 ");
					}
					else
					{
					System.out.println(" else stmt begin,id=9 ");
						try
						{
							   double [] var_162 = {(-1.06651e-124D),(-1.37118e-98D),(5.68824e-38D),(7.33075e+289D)};
						}
						catch( java.lang.ArrayIndexOutOfBoundsException myExp_163 )
						{
							var_39[5] = (+(--var_156)) ;
							System.out.println("hello exception 40 !");
						}
					System.out.println(" else stmt end,id=9 ");
					}
					System.out.println("hello exception 37 !");
				}
			System.out.println(" if stmt end,id=13 ");
			}
		}
		catch( java.lang.ArithmeticException myExp_164 )
		{
			   cl_96 var_165 = new cl_96();
			   cl_96 var_166 = new cl_96();
			var_155 = var_155 ;
			System.out.println("hello exception 42 !");
		}
	}
	return var_155.var_168;
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(63);
		CrcCheck.ToByteArray(this.var_33.GetChecksum(),b,"var_33.GetChecksum()");
		CrcCheck.ToByteArray(this.var_150.GetChecksum(),b,"var_150.GetChecksum()");
		CrcCheck.ToByteArray(this.var_151.GetChecksum(),b,"var_151.GetChecksum()");
		CrcCheck.ToByteArray(this.var_155.GetChecksum(),b,"var_155.GetChecksum()");
		CrcCheck.ToByteArray(this.var_156,b,"var_156");
		CrcCheck.ToByteArray(this.var_157,b,"var_157");
		CrcCheck.ToByteArray(this.var_177,b,"var_177");
		CrcCheck.ToByteArray(this.var_197.GetChecksum(),b,"var_197.GetChecksum()");
		CrcCheck.ToByteArray(this.var_270,b,"var_270");
		CrcCheck.ToByteArray(this.var_283.GetChecksum(),b,"var_283.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
