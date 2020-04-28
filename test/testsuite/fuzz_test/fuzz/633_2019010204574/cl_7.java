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

class cl_7 
{
	   cl_26 var_109 = new cl_26();
	   byte var_112 = (byte)(-63);
	   float var_113 = (-6.41385e-36F);
	   cl_60 var_114 = new cl_60();
	   int [] var_116 = {(-1613408244),(267527823),(301809805),(1853130894)};
	   int var_119 = (-952107546);
	final   cl_97 var_121 = new cl_97();
	final   cl_46 var_154 = new cl_46();
/*********************************/
public strictfp short func_27(int var_28, long var_29, long var_30)
{
	   byte var_31 = (byte)(-38);
	if( ((--var_31)>=func_32((!((--var_119)!=var_112)))))
	{
	// if stmt begin,id=9
		if( (!(!(!(((var_112++)+var_121.var_122)<=var_113)))))
		{
		// if stmt begin,id=10
			   int var_123 = (-1259631241);
			for( var_123 = 200 ;(var_123>152);var_123 = (var_123-12) )
			{
				var_116[3] = (++var_119) ;
			}
		// if stmt end,id=10
		}
		else
		{
		// else stmt begin,id=4
			final   double var_124 = (-8.23208e-242D);
		// else stmt end,id=4
		}
	// if stmt end,id=9
	}
	return (var_112++);
}
public strictfp byte func_32(boolean var_33)
{
	   byte var_117 = (byte)(56);
	   float [][] var_35 = {{(-3.73261e-05F)},{(1.47399e-11F)},{(-5.01839e+08F)},{(2.12673e+15F)},{(-1.63565e+17F)},{(-30125.2F)},{(-1.51715e+18F)}};
	   short var_110 = (short)(-3957);
	   cl_60 var_115 = new cl_60();
	var_35 = var_109.func_36((+(-(++var_110))) ,var_109.var_67.var_111 ,((var_112++)>=(4.00666e+43D)) ,((++var_113)>=var_114.var_57) ,var_115 ,var_116 ,(var_117--)) ;
	   byte var_118 = (byte)(27);
	return (var_118--);
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(57);
		CrcCheck.ToByteArray(this.var_109.GetChecksum(),b,"var_109.GetChecksum()");
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		CrcCheck.ToByteArray(this.var_113,b,"var_113");
		CrcCheck.ToByteArray(this.var_114.GetChecksum(),b,"var_114.GetChecksum()");
		for(int a0=0;a0<4;++a0){
			CrcCheck.ToByteArray(this.var_116[a0],b,"var_116" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_119,b,"var_119");
		CrcCheck.ToByteArray(this.var_121.GetChecksum(),b,"var_121.GetChecksum()");
		CrcCheck.ToByteArray(this.var_154.GetChecksum(),b,"var_154.GetChecksum()");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
