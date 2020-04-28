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

//SEED=3798900212

import java.util.zip.CRC32;

class MainClass 
{
	final  static cl_6 var_2 = new cl_6();
	  static cl_6 var_3 = new cl_6();
	  static cl_12 var_9 = new cl_12();
	  static byte var_24 = (byte)(-37);
	final  static byte var_25 = (byte)(121);
	  static int var_35 = (-1717571889);
	  static boolean [] var_36 = {false};
	  static int var_46 = (-1207869073);
	   double var_47 = (5.08727e+188D);
	  static byte var_50 = (byte)(-103);
	  static float [][][] var_51 = {{{(-1.06678e-06F),(1.38763e+35F),(227.247F),(-1.42302e-15F),(2.2153e-05F),(2.21186e-32F),(9.27424e-11F)},{(2.08657e-19F),(-1.45653e+27F),(3.86673e+10F),(-3.84833e-20F),(-4.85914e-32F),(9.08211e-27F),(-2.02706F)},{(-3.85502e+12F),(1.88205e+17F),(-1.20431e-32F),(3.31719e-10F),(9.46891e+31F),(-2.03298e+20F),(-7.36137e+36F)},{(-3.87542e+22F),(6.308e+10F),(2.66598e-10F),(-8.88015e+13F),(0.00955052F),(-0.000456214F),(-4.2275e+07F)},{(-9.2642e-29F),(-5724.98F),(-7.99928e+18F),(-2.18073e+15F),(8.96943e-27F),(-1.71376e-12F),(-3.51337e+37F)},{(-1.10779e-19F),(2.51917e-29F),(-0.00374927F),(-7.35413e+34F),(-7.8498e-22F),(2.89673e+15F),(-534.633F)},{(0.0010679F),(-11834F),(3.9041e+29F),(3.62786e+36F),(2.50764e-11F),(-2.39871e-20F),(1.20278e-20F)}},{{(5.37164e-15F),(-4.57348e-09F),(-3.03644e-37F),(2.37261e+17F),(-92282.9F),(3.75268e+21F),(2.91376e+30F)},{(-108446F),(-2.2236e-10F),(1.8593e-28F),(2.31449F),(-8.50776e+33F),(-1.08244e+26F),(-1.18964e+14F)},{(3.19862e-21F),(-1.33498e-19F),(-1.09646e-23F),(-1.16523e-26F),(8.4556e+11F),(-3.45369e-34F),(-5.89823e+06F)},{(4.42787e-15F),(-1.63836e+31F),(-9.96475e+24F),(-0.0433826F),(8.00225e-30F),(-4.30999e+12F),(-7.92349e+27F)},{(-0.00171125F),(-4.54196e+19F),(1.45339e-25F),(9.77189e+11F),(3.45285e-36F),(-1.71917e-23F),(-1.35222e+24F)},{(-3.09371e+12F),(2.63368e-17F),(-1.17026e+25F),(5.97715e+15F),(9.60143e+24F),(-1.57026e+21F),(2.98191e+23F)},{(-3.56343e-09F),(7.61662F),(840139F),(-6.24624e+16F),(-0.015985F),(-3.5913e+35F),(2.33659e-21F)}},{{(9.46058e-05F),(-890.489F),(-2.41778e-19F),(-8.94331e-23F),(1.81041e-27F),(-2.28524F),(7.22953e-22F)},{(0.871118F),(1.04557e-07F),(-2.07809e-08F),(-1.06906e+11F),(3.676e+07F),(-3.54474e+22F),(-3.98202e+07F)},{(5.7002e-10F),(5.14305e+17F),(-4.67731e+10F),(-1.14907e+34F),(-1.11805e-13F),(-7.71962e-11F),(-70.4809F)},{(-5.61817e+27F),(-2.48024e+20F),(-2.11287e+24F),(-1.87635e+34F),(1.53765e+29F),(-1.47776e+38F),(-1.98871e+24F)},{(2.07961e-24F),(1.68881e-33F),(-7.87842e-19F),(-6.49563e+15F),(-7.63583e-26F),(-6.60064e-10F),(-0.00742081F)},{(-2.61915e+23F),(3.18749e-12F),(-4.31801e-28F),(-1.07252e-10F),(-0.149113F),(-3.15607e-21F),(3.92382e+30F)},{(-2.08259e-38F),(-7.27439e-38F),(-1.07831e-05F),(1.85628e+34F),(1.77329e-23F),(2.1743e-06F),(-3.00768e+21F)}},{{(4.24073e+20F),(-4.06129e+12F),(8.24235e+20F),(-3.17688e-23F),(-130668F),(-7.2289e-27F),(-7.39855e-38F)},{(2.3542e-15F),(6.48835e-12F),(1.98966e+31F),(-2.85553e+28F),(1.06383e+25F),(9.48664e+12F),(-2.81379e-32F)},{(18340.4F),(-6.85172e+09F),(-9.30841e+24F),(-416.094F),(6.83238e-29F),(1.91703e-36F),(939.073F)},{(1.17452e+14F),(-8.03171e+34F),(-1.15518e-34F),(2.19478e-32F),(-3.82709e-25F),(-6.72072e-07F),(-1.91439e-26F)},{(-7.09161e+08F),(5.59208e-14F),(-1.20367e+22F),(-277849F),(-2.9902e+06F),(-7.60269e+09F),(7.77232e+30F)},{(1.04546e-05F),(-2.75024e-30F),(-1.26456e+07F),(6.69105F),(1.23347e-15F),(-777.485F),(15233.1F)},{(2.08915e-28F),(1.86698e+07F),(-1.25569e-09F),(3.65312e+36F),(4.98744e+32F),(-1.12721e-23F),(-1.58488e-06F)}},{{(1.18971e+07F),(4.00405e+20F),(-1.57164e+18F),(1.06011e-27F),(7.79265e-17F),(1.26492e-19F),(-2.8452e+29F)},{(5.83457e-34F),(1.1482e+35F),(-7469.2F),(3.19529e-22F),(1.6171e-06F),(-53738.3F),(2.9577e-22F)},{(5.83193e-39F),(3.06115e+29F),(-2.18221e-12F),(-2.69816e-18F),(0.0376036F),(-9.15188e-15F),(-3.47262e+08F)},{(-1.80942e+38F),(-0.0319564F),(-1.30207e+15F),(2.67319e-21F),(1.87461e+12F),(-1.14308e-08F),(-3.92858e+36F)},{(1.96433e+10F),(-1.52062e+06F),(1.96778e+36F),(-8.72511e-36F),(6.74325e+22F),(-5.44432e+14F),(-8.47681e+09F)},{(-5.71807e-08F),(-1.28489e+13F),(2.90839e+29F),(2.02351e+13F),(-4.9792e+30F),(-3.76719e+28F),(-4.92693e+07F)},{(-3.10484e-38F),(9.85802e-22F),(2.25565e+34F),(-0.00706959F),(-9.29492e+24F),(-195.555F),(2.78822e-35F)}},{{(-3.38707e+22F),(-6.11825e+11F),(8.81996e+24F),(7.98013e-22F),(-3.07295e-39F),(1.77058e-17F),(-1.74044e+11F)},{(3.46012e+24F),(2.81622e-25F),(3.73608e-27F),(-11078.7F),(1.85336e-38F),(-1520.79F),(2.90248e+18F)},{(2.44664e+19F),(-1.00714e-14F),(1.84842e+15F),(-2.68231e+29F),(-1.054e+21F),(-7.32016e-20F),(-3.59568e-06F)},{(-8.97708e+15F),(4.64513e-17F),(8.37812e+28F),(1.48403e-18F),(-3.20179e+07F),(6.59565e+35F),(-7.48617e-14F)},{(3.52347e-16F),(-4.72548e+33F),(-2.3183e-20F),(7.42875e-16F),(5.07272e+35F),(-36.9055F),(-5.04683e+12F)},{(1.05646e-07F),(4.04356e+27F),(7.66314e-07F),(-2.92243e+28F),(-1.58516e+29F),(-2.14329e+07F),(-1.35245e-27F)},{(-3.91893e+07F),(-3.04076e+08F),(2.46784e+21F),(-3.00495e-10F),(1.37957F),(1.86384e+36F),(2.01587e+38F)}}};
	  static byte var_55 = (byte)(-97);
	  static int var_56 = (-1020676575);
	   byte var_59 = (byte)(107);
	   byte var_62 = (byte)(-42);
	   short var_72 = (short)(16929);
	   cl_12 var_79 = new cl_12();
	   int var_88 = (-1532609251);
	   float var_89 = (-1.24851e-09F);
	   short var_93 = (short)(25042);
	   long var_98 = (5169903097905253256L);
/*********************************/
public strictfp void run()
{
	   int var_0 = (-1194430962);
	   cl_6 [] var_1 = {new cl_6(),new cl_6(),new cl_6(),new cl_6(),new cl_6(),new cl_6()};
	for( var_0 = 478 ;(var_0<505);var_0 = (var_0+9) )
	{
		   short var_97 = (short)(32544);
		var_1[(var_0&5)] = func_4(((var_97++)&var_98) ,var_9 ,var_9.var_101 ,func_10()) ;
	}
	return ;
}
public strictfp cl_6 func_4(long var_5, cl_12 var_6, int[][] var_7, cl_12 var_8)
{
	   short var_86 = (short)(29905);
	   byte var_94 = (byte)(37);
	   cl_6 var_95 = new cl_6();
	try
	{
		var_8 = func_10() ;
	}
	catch( java.lang.ArrayIndexOutOfBoundsException myExp_85 )
	{
		   byte var_87 = (byte)(122);
		var_7[((var_87--)&0)][(((var_86--)&var_2.var_43)&2)] = ((var_88--)&var_88) ;
		System.out.println("hello exception 0 !");
	}
	return func_13((var_89--) ,var_79 ,(var_72++) ,func_13((var_93++) ,var_79 ,(~(var_94--)) ,var_95));
}
public strictfp cl_12 func_10()
{
	   float var_82 = (5.95904e-16F);
	   int var_11 = (143130509);
	for( var_11 = 555 ;(var_11>460);var_11 = (var_11-19) )
	{
		final   boolean var_12 = true;
		try
		{
			var_3 = func_13(((var_72++)&var_3.var_78) ,var_79 ,(+(var_56--)) ,var_3) ;
		}
		catch( java.lang.ArrayIndexOutOfBoundsException myExp_80 )
		{
			try
			{
				var_2.var_81 = (var_82++) ;
			}
			catch( java.lang.IllegalArgumentException myExp_83 )
			{
				   int [][] var_84 = {{(279906333),(-1328656641)},{(1874700458),(2112067771)},{(1280901849),(-2100562391)}};
				System.out.println("hello exception 2 !");
			}
			System.out.println("hello exception 1 !");
		}
	}
	return var_79;
}
public strictfp cl_6 func_13(float var_14, cl_12 var_15, long var_16, cl_6 var_17)
{
	   float var_61 = (-8.26975e-23F);
	   short var_71 = (short)(-15010);
	   boolean [][] var_57 = {{true,true,false,false,true,true,false},{false,true,true,false,true,true,false},{false,false,true,true,false,false,true},{false,false,true,false,true,false,false},{true,true,false,false,true,true,false}};
	   byte var_18 = (byte)(-29);
	if( (!((var_18--)>func_19(var_57[((var_59--)&4)][(((((var_55--)|( ( int )(var_50--) ))*var_9.var_58)-var_46)&6)] ,((+(++var_61))<=( ( short )(var_62++) )) ,var_36))))
	{
		   short var_67 = (short)(12192);
		try
		{
			try
			{
				   short var_65 = (short)(11932);
				var_56 = (--var_65) ;
			}
			catch( java.lang.ArithmeticException myExp_66 )
			{
				var_51[((var_67++)&5)][((--var_18)&6)][((~(++var_18))&6)] = var_51[5][(((--var_24)+var_9.var_68)&6)][((+(++var_67))&6)] ;
				System.out.println("hello exception 3 !");
			}
		}
		catch( java.lang.ArrayIndexOutOfBoundsException myExp_69 )
		{
			   int var_70 = (1498923209);
			for( var_70 = 1014 ;(var_70<1044);var_70 = (var_70+10) )
			{
				var_9.var_58 = ((--var_67)-var_9.var_28) ;
			}
			System.out.println("hello exception 4 !");
		}
	}
	else
	{
		try
		{
			try
			{
				var_57[(((var_71--)*(short)(27810))&4)][((var_18++)&6)] = (!((++var_62)>=var_72)) ;
			}
			catch( java.lang.ArithmeticException myExp_73 )
			{
				var_57[((+(var_24--))&4)][((--var_18)&6)] = (!((var_18--)<=var_9.var_58)) ;
				System.out.println("hello exception 5 !");
			}
		}
		catch( java.lang.ArrayIndexOutOfBoundsException myExp_74 )
		{
			if( ( ( boolean )((+((var_35--)|( ( byte )(--var_72) )))==func_19((!((++var_18)<=var_72)) ,((++var_50)<var_2.var_43) ,var_36)) ))
			{
				var_3 = var_3 ;
			}
			else
			{
				var_3 = var_2 ;
			}
			System.out.println("hello exception 6 !");
		}
	}
	return var_17;
}
public static strictfp double func_19(boolean var_20, boolean var_21, boolean[] var_22)
{
	   int var_54 = (-1223581849);
	final   byte [] var_33 = {(byte)(-23),(byte)(-113),(byte)(71),(byte)(-78),(byte)(-70),(byte)(-49),(byte)(12)};
	   short var_53 = (short)(-23588);
	   byte var_52 = (byte)(89);
	try
	{
		   short var_39 = (short)(-18216);
		   int var_23 = (-907756238);
		   int var_30 = (-1342020718);
		for( var_23 = 975 ;(var_23<978);var_23 = (var_23+1) )
		{
			   byte var_31 = (byte)(24);
			if( (!((-((++var_24)&var_25))<=(-1346463990))))
			{
				   int var_27 = (-845507212);
				   cl_6 var_32 = new cl_6();
				var_2.var_26 = ((var_27--)&var_9.var_28) ;
				   short var_34 = (short)(-18577);
				var_22[((+(++var_30))&0)] = false ;
				   float [] var_41 = {(-1.44496e-15F),(-8.5128e+24F),(7.51734e+27F),(-1.50722e+12F),(8.19875e-13F),(-2.07148e-10F)};
				var_24 = (--var_31) ;
				   boolean var_44 = true;
				   cl_6 var_38 = new cl_6();
				var_31 = (++var_31) ;
				var_22 = var_36 ;
				final   long var_42 = (-3658093695489938897L);
				var_30 = ((var_24++)*var_24) ;
				var_21 = (!((var_39++)==var_9.var_40)) ;
				var_3 = var_2 ;
				var_35 = ((--var_24)*var_2.var_43) ;
				var_22[((var_30--)&0)] = ((var_31--)>=( ( byte )(-(var_24--)) )) ;
			}
		}
	}
	catch( java.lang.ArithmeticException myExp_45 )
	{
		for( var_46 = 476 ;(var_46<491);var_46 = (var_46+3) )
		{
			try
			{
				final   boolean var_48 = true;
			}
			catch( java.lang.ArithmeticException myExp_49 )
			{
				var_22[((+(++var_50))&0)] = var_22[((--var_24)&0)] ;
				System.out.println("hello exception 8 !");
			}
		}
		System.out.println("hello exception 7 !");
	}
	return (~(((-(--var_50))-( ( long )var_51[((((++var_55)-var_56)&var_35)&5)][(((--var_54)&var_9.var_28)&6)][(((++var_52)+( ( int )(var_53--) ))&6)] ))*var_2.var_26));
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(1255);
		CrcCheck.ToByteArray(this.var_2.GetChecksum(),b,"var_2.GetChecksum()");
		CrcCheck.ToByteArray(this.var_3.GetChecksum(),b,"var_3.GetChecksum()");
		CrcCheck.ToByteArray(this.var_9.GetChecksum(),b,"var_9.GetChecksum()");
		CrcCheck.ToByteArray(this.var_24,b,"var_24");
		CrcCheck.ToByteArray(this.var_25,b,"var_25");
		CrcCheck.ToByteArray(this.var_35,b,"var_35");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_36[a0],b,"var_36" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_46,b,"var_46");
		CrcCheck.ToByteArray(this.var_47,b,"var_47");
		CrcCheck.ToByteArray(this.var_50,b,"var_50");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_51[a0][a1][a2],b,"var_51" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_62,b,"var_62");
		CrcCheck.ToByteArray(this.var_72,b,"var_72");
		CrcCheck.ToByteArray(this.var_79.GetChecksum(),b,"var_79.GetChecksum()");
		CrcCheck.ToByteArray(this.var_88,b,"var_88");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_93,b,"var_93");
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
