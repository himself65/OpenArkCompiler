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

//SEED=2472495084

//import java.util.zip.CRC32;

class cl_11 
{
	   short var_12 = (short)(-10474);
	   cl_7 var_125 = new cl_7();
	   byte var_126 = (byte)(-18);
	   long [] var_127 = {(3067312195741181929L),(-5365055639171591251L),(-4810499107680539658L),(5186041432998772531L),(3676209241845108406L),(4424218253840078406L),(-3505100354285504352L)};
	   cl_26 var_129 = new cl_26();
	   cl_46 var_130 = new cl_46();
	   byte var_133 = (byte)(-55);
	   cl_60 [][] var_135 = {{new cl_60()}};
	   byte var_136 = (byte)(-13);
	   cl_97 var_138 = new cl_97();
	final   cl_60 var_151 = new cl_60();
	   int var_153 = (1485821349);
	   double var_156 = (4.77265e+42D);
	   int var_157 = (1385862701);
/*********************************/
public strictfp long func_22(boolean var_23, float var_24, cl_46 var_25)
{
	   short var_128 = (short)(14567);
	   cl_60 var_145 = new cl_60();
	   int var_158 = (-564793044);
	   cl_60 var_147 = new cl_60();
	   cl_60 var_148 = new cl_60();
	try
	{
		   byte var_26 = (byte)(12);
		if( ((++var_26)>var_125.func_27((+(var_126++)) ,var_127[((var_128--)&6)] ,(++var_12))))
		{
		// if stmt begin,id=11
			var_24 = (((-(var_26--))*var_129.var_89)*var_130.var_131.var_49.var_55) ;
		// if stmt end,id=11
		}
	}
	catch( java.lang.ArithmeticException myExp_132 )
	{
		   cl_26 var_155 = new cl_26();
		   cl_60 var_140 = new cl_60();
		final   cl_60 var_146 = new cl_60();
		   cl_60 var_150 = new cl_60();
		if( (!(!(!(!(!((var_133--)<=( ( short )(--var_133) ))))))))
		{
		// if stmt begin,id=12
			   cl_60 var_141 = new cl_60();
			   byte var_137 = (byte)(75);
			try
			{
				   cl_60 var_143 = new cl_60();
				   cl_60 var_144 = new cl_60();
				final   cl_60 var_149 = new cl_60();
				final   cl_60 var_139 = new cl_60();
				   cl_60 var_142 = new cl_60();
				var_135[(((var_12--)-var_138.var_68)&0)][(((+(var_136--))+( ( int )(var_137--) ))&0)] = var_145 ;
			}
			catch( java.lang.ArrayIndexOutOfBoundsException myExp_152 )
			{
				for( var_153 = 717 ;(var_153>697);var_153 = (var_153-4) )
				{
					var_129 = var_155 ;
				}
				System.out.println("hello exception 4 !");
			}
		// if stmt end,id=12
		}
		else
		{
		// else stmt begin,id=5
			if( (!(!((-(++var_128))<=var_156))))
			{
			// if stmt begin,id=13
				for( var_157 = 468 ;(var_157<478);var_157 = (var_157+5) )
				{
					var_127[(var_157&6)] = (~(++var_126)) ;
				}
			// if stmt end,id=13
			}
		// else stmt end,id=5
		}
		System.out.println("hello exception 3 !");
	}
	return (++var_158);
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(125);
		CrcCheck.ToByteArray(this.var_12,b,"var_12");
		CrcCheck.ToByteArray(this.var_125.GetChecksum(),b,"var_125.GetChecksum()");
		CrcCheck.ToByteArray(this.var_126,b,"var_126");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_127[a0],b,"var_127" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_129.GetChecksum(),b,"var_129.GetChecksum()");
		CrcCheck.ToByteArray(this.var_130.GetChecksum(),b,"var_130.GetChecksum()");
		CrcCheck.ToByteArray(this.var_133,b,"var_133");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<1;++a1){
			CrcCheck.ToByteArray(this.var_135[a0][a1].GetChecksum(),b,"var_135" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_136,b,"var_136");
		CrcCheck.ToByteArray(this.var_138.GetChecksum(),b,"var_138.GetChecksum()");
		CrcCheck.ToByteArray(this.var_151.GetChecksum(),b,"var_151.GetChecksum()");
		CrcCheck.ToByteArray(this.var_153,b,"var_153");
		CrcCheck.ToByteArray(this.var_156,b,"var_156");
		CrcCheck.ToByteArray(this.var_157,b,"var_157");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
