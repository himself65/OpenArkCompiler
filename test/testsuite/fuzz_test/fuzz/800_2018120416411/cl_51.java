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

//SEED=1217165572

//import java.util.zip.CRC32;

class cl_51 
{
	  static int var_43 = (1824443085);
	  static byte var_45 = (byte)(37);
	  static cl_8 var_47 = new cl_8();
	  static cl_16 var_49 = new cl_16();
	  static float var_56 = (-6.20332e-34F);
	  static cl_59 var_59 = new cl_59();
	  static cl_19 var_62 = new cl_19();
	  static short var_68 = (short)(8279);
	  static float var_72 = (-7285.99F);
	  static cl_12 var_80 = new cl_12();
	  static int var_85 = (-1062313940);
	  static cl_19 [][][] var_91 = {{{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()},{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()},{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()}},{{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()},{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()},{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()}},{{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()},{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()},{new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19(),new cl_19()}}};
	  static double [][] var_92 = {{(-5.19837e+250D),(-3.16396e+62D)},{(4.53764e-282D),(7.92634e-247D)},{(3.14412e+140D),(-1.05796e+219D)},{(-1.03641e-277D),(-1.05493e-162D)},{(-1.0929e-226D),(-1.26384e+40D)},{(-1.83286e+66D),(-2.23766e+68D)}};
	  static double var_94 = (3.63809e-194D);
	   cl_16 var_96 = new cl_16();
	  static int var_115 = (-184636284);
	  static cl_16 [][] var_116 = {{new cl_16(),new cl_16()}};
	final   cl_16 [][] var_117 = {{new cl_16(),new cl_16()}};
/*********************************/
public static strictfp boolean func_37(boolean var_38, boolean var_39, cl_59[][][] var_40, boolean var_41, int var_42)
{
	   cl_59 var_87 = new cl_59();
	   cl_12 var_126 = new cl_12();
	for( var_43 = 374 ;(var_43<380);var_43 = (var_43+2) )
	{
		   int var_52 = (-163071080);
		   int var_78 = (808197840);
		   byte var_44 = (byte)(27);
		if( ((++var_44)>( ( float )(++var_45) )))
		{
		System.out.println(" if stmt begin,id=38 ");


























































































































































		System.out.println(" if stmt end,id=38 ");
		}
		else
		{
		System.out.println(" else stmt begin,id=16 ");










































































































		System.out.println(" else stmt end,id=16 ");
		}
	}
	return ((var_45--)!=( ( int )(--var_45) ));
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(639);
		CrcCheck.ToByteArray(this.var_43,b,"var_43");
		CrcCheck.ToByteArray(this.var_45,b,"var_45");
		CrcCheck.ToByteArray(this.var_47.GetChecksum(),b,"var_47.GetChecksum()");
		CrcCheck.ToByteArray(this.var_49.GetChecksum(),b,"var_49.GetChecksum()");
		CrcCheck.ToByteArray(this.var_56,b,"var_56");
		CrcCheck.ToByteArray(this.var_59.GetChecksum(),b,"var_59.GetChecksum()");
		CrcCheck.ToByteArray(this.var_62.GetChecksum(),b,"var_62.GetChecksum()");
		CrcCheck.ToByteArray(this.var_68,b,"var_68");
		CrcCheck.ToByteArray(this.var_72,b,"var_72");
		CrcCheck.ToByteArray(this.var_80.GetChecksum(),b,"var_80.GetChecksum()");
		CrcCheck.ToByteArray(this.var_85,b,"var_85");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_91[a0][a1][a2].GetChecksum(),b,"var_91" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_92[a0][a1],b,"var_92" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_94,b,"var_94");
		CrcCheck.ToByteArray(this.var_96.GetChecksum(),b,"var_96.GetChecksum()");
		CrcCheck.ToByteArray(this.var_115,b,"var_115");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_116[a0][a1].GetChecksum(),b,"var_116" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_117[a0][a1].GetChecksum(),b,"var_117" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
