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

//SEED=1595514112

import java.util.zip.CRC32;

class MainClass 
{
	  static byte var_3 = (byte)(18);
	  static cl_9 var_5 = new cl_9();
	  static byte var_9 = (byte)(105);
	  static long var_11 = (4161532736081967335L);
	  static byte var_13 = (byte)(-102);
	  static int var_16 = (-1522402437);
	  static cl_38 var_22 = new cl_38();
	  static float [][][] var_23 = {{{(0.00848503F),(1.00384e+15F),(8.71853e-18F),(1.84957e-12F),(-5.27505e+06F),(-1.63963e-12F)},{(-9.26624e-29F),(-2.94897e-22F),(-0.00212667F),(1.09413e-14F),(-5.14647e-29F),(-2.7418e+20F)},{(4.27499e+23F),(9.90969e+26F),(2.19793e-05F),(-2.31779e-21F),(-6.25542e-13F),(-1.00226e+18F)},{(1.24278e-29F),(6.68537e-08F),(1.75566e-16F),(3.22681e+16F),(-1.31349e+29F),(-1.23907e+15F)},{(-9.94582e+07F),(-8.97219e+22F),(-4.43838e-30F),(-2.19858e+38F),(8.44428e-35F),(3.14113e+35F)}},{{(-12.8831F),(0.221322F),(-1.41005e-24F),(-1.01131e-29F),(-9.03956e-05F),(9.39518e+14F)},{(-3.18579e+21F),(2.95155e+17F),(6.07236e+19F),(-1.46357e+18F),(-3.76036e-18F),(-5.89672e-23F)},{(-5.25992e-37F),(7.55779e+13F),(-2.8475e+28F),(-2.23066e-25F),(1.22672e+38F),(2.35847e+33F)},{(-4.42688e-29F),(1.81037e+32F),(-1.34705e-16F),(2.07837e-23F),(2.28997e+21F),(2.05998e+38F)},{(-8.4691e+14F),(7.02539e+35F),(3.74185e-31F),(-1.71868e-08F),(1.64436e-19F),(-2.68183e-08F)}},{{(-3.20933F),(9.54228e-20F),(-0.00560349F),(-1.58483e+14F),(7.82671e-37F),(8.86914e+16F)},{(1.44445e-18F),(-8.56587e-19F),(7.75535e-10F),(-1.0948e-16F),(0.00506286F),(1.73177e-26F)},{(2.63367e-24F),(1.83811e+15F),(-840.62F),(2.0805e+12F),(3.52213e-06F),(8.24046e-05F)},{(6.06742e+30F),(-1.18098e-23F),(3.22977e-20F),(-3.24002e-37F),(-48.8912F),(-1.25173e-24F)},{(-3.91229e+37F),(-9.60536e-28F),(8.73533e-30F),(1.11446e-16F),(-28706.9F),(-543.102F)}},{{(1.64258e+31F),(3.99345e-27F),(-8.41663e-16F),(2.92898e-12F),(1.04744e-25F),(-2.42787e+17F)},{(-5.00629e-14F),(-972.069F),(9.31803e+34F),(4.27775e+25F),(6.57614e+18F),(-6.72027e+15F)},{(-1.16761e-08F),(-7.513e+31F),(-1.76898e+29F),(3.31408e+12F),(-9.3867e-24F),(-1.28495e+16F)},{(-8.46378e+15F),(-4062.93F),(112.92F),(0.310715F),(1.7772e-29F),(0.00141826F)},{(1.42202e-18F),(4.89175e+24F),(-1.98825e-25F),(-8.25809e-22F),(-1.24475e-11F),(-3.69602e-25F)}}};
	  static int [][] var_27 = {{(-1449199465),(1561796482),(-1871254310)},{(1845263420),(983728690),(-870729868)}};
	  static int var_29 = (2097175590);
	  static short var_36 = (short)(-29008);
	  static int var_37 = (-1345106889);
	  static byte var_38 = (byte)(86);
	final  static long [][][] var_41 = {{{(8687446987827591988L),(8183050042331208920L),(4525277741121595428L),(-7195058578064536678L)}},{{(8440438572674079026L),(-8991671311670472800L),(8070778345104785093L),(-7592823406259566275L)}}};
	   long var_45 = (-1979934987403528192L);
	  static cl_38 [][][] var_48 = {{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}}};
	   boolean var_52 = false;
	  static byte var_58 = (byte)(-15);
	  static byte var_59 = (byte)(-78);
	  static long var_64 = (3149434359261929369L);
	  static short var_67 = (short)(-7314);
	   int var_72 = (1205103317);
	   short var_73 = (short)(842);
	   int var_75 = (-463973107);
	   int var_76 = (-936922860);
	   int [][][] var_78 = {{{(-148856301)},{(1986320307)},{(987543930)},{(52107708)}},{{(-1716211236)},{(-501857499)},{(383949275)},{(-252916666)}}};
	   byte var_79 = (byte)(68);
	   float var_85 = (1.83241e-28F);
	   short var_90 = (short)(7845);
	   short var_91 = (short)(-31792);
/*********************************/
public strictfp void run()
{
	try
	{
		   int var_0 = (-927239616);
		for( var_0 = 708 ;(var_0>696);var_0 = (var_0-6) )
		{
			   int var_2 = (-824555225);
			try
			{
				   int [] var_1 = {(-1301604926),(664848274),(-1734777042)};
				   double var_4 = (2.37567e+272D);
				var_2 = ((var_3++)-var_3) ;
				var_5 = func_6() ;
			}
			catch( java.lang.ArrayIndexOutOfBoundsException myExp_83 )
			{
				var_5 = func_6() ;
				System.out.println("hello exception 0 !");
			}
		}
	}
	catch( java.lang.IllegalArgumentException myExp_84 )
	{
		   int var_86 = (-1297843178);
		try
		{
			var_85 = (var_86--) ;
		}
		catch( java.lang.ArrayIndexOutOfBoundsException myExp_87 )
		{
			try
			{
				   boolean [][][] var_89 = {{{true,true,true,false,true,false},{true,false,true,true,false,true}}};
				   byte var_92 = (byte)(-118);
				   short var_93 = (short)(-11528);
				var_89[((--var_92)&0)][((var_91--)&1)][((var_90++)&5)] = ((((var_93++)-var_72)!=var_36)&&func_28()) ;
			}
			catch( java.lang.IllegalArgumentException myExp_94 )
			{
				   cl_9 var_95 = new cl_9();
				System.out.println("hello exception 3 !");
			}
			System.out.println("hello exception 2 !");
		}
		System.out.println("hello exception 1 !");
	}
	return ;
}
public strictfp cl_9 func_6()
{
	final   cl_9 var_82 = new cl_9();
	if( (!(!(!(!(!(!var_5.var_7)))))))
	{
		   int var_8 = (-874888418);
		   cl_38 [][] var_71 = {{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}};
		   cl_38 var_74 = new cl_38();
		for( var_8 = 690 ;(var_8<754);var_8 = (var_8+16) )
		{
			try
			{
				   float [][] var_10 = {{(3.36015e+28F),(1.00198e-36F),(-1.4918e-07F),(381731F),(-4.8965e-33F)}};
				var_3 = (var_9++) ;
				var_11 = (((--var_13)-func_14())&var_9) ;
			}
			catch( java.lang.ArithmeticException myExp_69 )
			{
				var_71[((var_73++)&0)][((var_72--)&4)] = var_22 ;
				System.out.println("hello exception 4 !");
			}
		}
	}
	else
	{
		for( var_75 = 666 ;(var_75<671);var_75 = (var_75+5) )
		{
			   byte var_81 = (byte)(-128);
			for( var_76 = 929 ;(var_76<971);var_76 = (var_76+14) )
			{
				   byte var_80 = (byte)(-13);
				var_78[(((--var_81)+(short)(-17106))&1)][((+(var_80--))&3)][((++var_79)&0)] = ((var_81++)&var_22.var_54) ;
			}
		}
	}
	return var_82;
}
public static strictfp short func_14()
{
	var_5.var_15 = (var_11--) ;
	   short var_68 = (short)(-27898);
	for( var_16 = 871 ;(var_16<875);var_16 = (var_16+1) )
	{
		   byte var_20 = (byte)(24);
		   int var_17 = (657470767);
		   float [][][] var_19 = {{{(1.55239e-14F),(-1.87897e-32F),(2.62125e-36F),(1.34161e+29F),(-0.000252102F),(9.33828e+33F)},{(8.01129e+36F),(-6.53645e-33F),(-2.10594e-30F),(4.81023e+21F),(-1.04582e-31F),(-8.99897e-14F)},{(-2.70708e-28F),(-1.74251e+27F),(2.73091F),(1.60043e+13F),(-1.0634e-37F),(1.298e-11F)},{(1.85707e-11F),(-3.34667e+25F),(-189.732F),(2.86555e+10F),(7.17774e-35F),(8.44039e-26F)},{(3.20101e-38F),(-3.20781e-30F),(-1.22492e+06F),(-1.47151e+33F),(-7.71931e-31F),(-7.29837e+08F)}},{{(-5.54315e-07F),(-3.70304e-39F),(-1.30949e+34F),(5.44743e-10F),(1.49671e+06F),(-106.9F)},{(3.8271e-24F),(-4.85559e+15F),(5.60199e-33F),(2.36388e-06F),(-9.66998e-16F),(-4.93297e+24F)},{(-8.39475e-39F),(1.46551e-28F),(-3.183e-19F),(-2.02397e+29F),(-8.00055e-19F),(1.84724e+14F)},{(427697F),(4.39403e+21F),(1.8483e-26F),(-1.21584e-31F),(3.46513e-31F),(8.04475e+08F)},{(1.35455e-06F),(0.00032207F),(-2.80209e-36F),(-1.50899e-08F),(3.79774e-38F),(-9.56024e+32F)}},{{(-3.4335e+21F),(1.28873e+19F),(5.62281e+13F),(-2.58298e+20F),(1.00764e-13F),(-3.53748e+06F)},{(-0.000543092F),(0.33945F),(-2.1049e-21F),(2.50713e-17F),(-3.97525e+23F),(-2.8528e+14F)},{(3.65536e-31F),(-0.759776F),(-7.7166e-31F),(-1.83982e+25F),(-3.78825e-25F),(-2.31403e+26F)},{(7.50071e+15F),(5.5054e-12F),(6.62991e+34F),(-3.50666e+11F),(2.28577e-11F),(3.81989e+29F)},{(-9.73936e-37F),(-1.09928e+24F),(-1.36422e+23F),(-3.27167e+13F),(1.27209e-11F),(-5.94027e-17F)}},{{(-6.91117e+32F),(1.53937e+07F),(-1.69336e+13F),(-2.50169e+14F),(4.65331e-30F),(3.83211e+11F)},{(7.73635e+12F),(-623.298F),(-1.96043e-32F),(-5.85369e-23F),(12785.6F),(-8.97469e+15F)},{(-2.67562e+32F),(2.5587e+11F),(-2.91134e-34F),(-1.84119e+09F),(7.36509e+06F),(-2.2733e-30F)},{(0.000255515F),(-5.38913e-17F),(1.17221e-20F),(1296.56F),(-6.30467e+25F),(-1.13216e+07F)},{(5.6306e-31F),(3.66163e+21F),(1.78059e+25F),(5.13864e-26F),(7.37479e+23F),(7.57501e-07F)}}};
		for( var_17 = 878 ;(var_17<895);var_17 = (var_17+17) )
		{
			   int var_21 = (1427206282);
			var_19[((var_21++)&3)][((+(++var_20))&4)][(((++var_20)|var_16)&5)] = ((--var_20)+var_5.var_15) ;
			try
			{
				var_19 = func_24((var_64++)) ;
			}
			catch( java.lang.ArithmeticException myExp_65 )
			{
				var_22.var_66 = ((++var_67)+var_22.var_66) ;
				System.out.println("hello exception 5 !");
			}
		}
	}
	return (++var_68);
}
public static strictfp float[][][] func_24(float var_25)
{
	var_5.var_26 = var_27 ;
	   float [][][] var_57 = {{{(7.28096e-06F),(-2.18878e+32F),(-1.6805e+29F),(-0.00380722F),(4.30521e+20F),(8.04701e+06F)}},{{(-4.47326e-15F),(4.18305e+34F),(3.74492e-06F),(1.65708e+38F),(6.51601e+18F),(-4.81153e+20F)}}};
	if( func_28())
	{
		   short var_62 = (short)(28441);
		try
		{
			   long var_60 = (1436371912401526998L);
			try
			{
				var_57[(((--var_58)+var_22.var_39)&1)][((var_59++)&0)][((-(--var_58))&5)] = (var_60++) ;
			}
			catch( java.lang.ArrayIndexOutOfBoundsException myExp_61 )
			{
				var_5.var_43 = ((var_62++)&var_5.var_12) ;
				System.out.println("hello exception 6 !");
			}
		}
		catch( java.lang.ArithmeticException myExp_63 )
		{
			var_57[((-(var_9++))&1)][((var_9++)&0)][((++var_37)&5)] = (+((var_11++)*var_58)) ;
			System.out.println("hello exception 7 !");
		}
	}
	return var_23;
}
public static strictfp boolean func_28()
{
	   byte var_47 = (byte)(47);
	final   boolean [][][] var_34 = {{{false},{false},{false},{true},{false},{true}}};
	   short var_55 = (short)(-6938);
	for( var_29 = 542 ;(var_29<547);var_29 = (var_29+5) )
	{
		   int var_30 = (-1300022882);
		   double var_44 = (-2.6459e+191D);
		   cl_38 [][][] var_49 = {{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}},{{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()},{new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38(),new cl_38()}}};
		   short var_42 = (short)(-13734);
		for( var_30 = 799 ;(var_30>745);var_30 = (var_30-18) )
		{
			   boolean [][][] var_33 = {{{true},{true},{false},{true},{false},{true}}};
			   byte var_40 = (byte)(82);
			   int var_31 = (2088612453);
			for( var_31 = 778 ;(var_31<786);var_31 = (var_31+8) )
			{
				final   int var_32 = (2089359583);
				   long var_35 = (-3120176112799566210L);
				var_22 = var_22 ;
				var_33[(var_29&0)][(var_30&5)][(var_31&0)] = true ;
				var_33 = var_34 ;
				var_23[(var_29&3)][(var_30&4)][(var_31&5)] = (var_35++) ;
				var_33[0][((var_37--)&5)][((--var_36)&0)] = ((((var_38++)*var_22.var_39)&( ( int )(--var_40) ))>=( ( byte )((var_40--)*( ( int )var_41[1][((-(--var_42))&0)][((var_40--)&3)] )) )) ;
				var_33[((var_36++)&0)][((var_3--)&5)][((var_13--)&0)] = (!(!(!(!(((var_40++)-var_5.var_43)==var_36))))) ;
				var_42 = (++var_42) ;
				var_33[(var_29&0)][(var_30&5)][(var_31&0)] = (!((++var_44)>=var_36)) ;
				var_33[(var_29&0)][(var_30&5)][(var_31&0)] = false ;
			}
		}
		try
		{
			try
			{
				var_5.var_46 = (((++var_47)*var_9)-var_3) ;
				var_48 = var_48 ;
				   cl_9 var_50 = new cl_9();
				var_5 = var_50 ;
			}
			catch( java.lang.ArrayIndexOutOfBoundsException myExp_51 )
			{
				var_3 = (--var_47) ;
				System.out.println("hello exception 8 !");
			}
		}
		catch( java.lang.IllegalArgumentException myExp_53 )
		{
			if( ((var_42--)>=var_22.var_54))
			{
				var_47 = (--var_3) ;
			}
			System.out.println("hello exception 9 !");
		}
	}
	return (!(!(!((var_55++)!=var_36))));
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(2030);
		CrcCheck.ToByteArray(this.var_3,b,"var_3");
		CrcCheck.ToByteArray(this.var_5.GetChecksum(),b,"var_5.GetChecksum()");
		CrcCheck.ToByteArray(this.var_9,b,"var_9");
		CrcCheck.ToByteArray(this.var_11,b,"var_11");
		CrcCheck.ToByteArray(this.var_13,b,"var_13");
		CrcCheck.ToByteArray(this.var_16,b,"var_16");
		CrcCheck.ToByteArray(this.var_22.GetChecksum(),b,"var_22.GetChecksum()");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_23[a0][a1][a2],b,"var_23" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_27[a0][a1],b,"var_27" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_29,b,"var_29");
		CrcCheck.ToByteArray(this.var_36,b,"var_36");
		CrcCheck.ToByteArray(this.var_37,b,"var_37");
		CrcCheck.ToByteArray(this.var_38,b,"var_38");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_41[a0][a1][a2],b,"var_41" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_45,b,"var_45");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_48[a0][a1][a2].GetChecksum(),b,"var_48" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_64,b,"var_64");
		CrcCheck.ToByteArray(this.var_67,b,"var_67");
		CrcCheck.ToByteArray(this.var_72,b,"var_72");
		CrcCheck.ToByteArray(this.var_73,b,"var_73");
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_76,b,"var_76");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_78[a0][a1][a2],b,"var_78" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_79,b,"var_79");
		CrcCheck.ToByteArray(this.var_85,b,"var_85");
		CrcCheck.ToByteArray(this.var_90,b,"var_90");
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
