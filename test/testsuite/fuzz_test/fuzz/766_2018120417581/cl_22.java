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

//SEED=2759125148

//import java.util.zip.CRC32;

class cl_22 
{
	  static boolean [][] var_39 = {{true,false},{false,false},{false,false},{true,false},{true,false},{true,true},{false,false}};
	  static cl_13 var_142 = new cl_13();
	  static cl_39 var_144 = new cl_39();
	  static cl_76 var_146 = new cl_76();
	  static cl_95 var_149 = new cl_95();
	  static short var_151 = (short)(-20815);
	  static short var_153 = (short)(19916);
	   cl_76 [] var_186 = {new cl_76(),new cl_76(),new cl_76(),new cl_76(),new cl_76(),new cl_76(),new cl_76()};
	final   cl_42 var_192 = new cl_42();
	   cl_95 [][] var_202 = {{new cl_95(),new cl_95(),new cl_95(),new cl_95(),new cl_95(),new cl_95()},{new cl_95(),new cl_95(),new cl_95(),new cl_95(),new cl_95(),new cl_95()},{new cl_95(),new cl_95(),new cl_95(),new cl_95(),new cl_95(),new cl_95()}};
	final   double var_239 = (-9.21167e+228D);
	   float var_258 = (1.64543e+06F);
	   cl_76 [][] var_325 = {{new cl_76(),new cl_76(),new cl_76(),new cl_76()},{new cl_76(),new cl_76(),new cl_76(),new cl_76()},{new cl_76(),new cl_76(),new cl_76(),new cl_76()},{new cl_76(),new cl_76(),new cl_76(),new cl_76()},{new cl_76(),new cl_76(),new cl_76(),new cl_76()}};
	   byte var_437 = (byte)(-58);
	final   int var_447 = (-539514874);
/*********************************/
public static strictfp int func_31(cl_39 var_32, float var_33, cl_42 var_34, int var_35, long var_36, double var_37)
{
	   int var_152 = (101080247);
	   byte var_148 = (byte)(114);
	   short var_145 = (short)(-13194);
	   byte var_143 = (byte)(2);
	   byte var_40 = (byte)(-126);
	var_39[((var_152++)&6)][((((--var_40)|var_142.func_41(((++var_143)-var_144.var_94) ,((var_145++)*var_146.var_147) ,(+((++var_148)|var_149.var_150)) ,(var_35++)))|var_151)&1)] = true ;
	return (-(var_153--));
}

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(435);
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_39[a0][a1],b,"var_39" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_142.GetChecksum(),b,"var_142.GetChecksum()");
		CrcCheck.ToByteArray(this.var_144.GetChecksum(),b,"var_144.GetChecksum()");
		CrcCheck.ToByteArray(this.var_146.GetChecksum(),b,"var_146.GetChecksum()");
		CrcCheck.ToByteArray(this.var_149.GetChecksum(),b,"var_149.GetChecksum()");
		CrcCheck.ToByteArray(this.var_151,b,"var_151");
		CrcCheck.ToByteArray(this.var_153,b,"var_153");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_186[a0].GetChecksum(),b,"var_186" + "["+ Integer.toString(a0)+"]"+".GetChecksum()");
		}
		CrcCheck.ToByteArray(this.var_192.GetChecksum(),b,"var_192.GetChecksum()");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<6;++a1){
			CrcCheck.ToByteArray(this.var_202[a0][a1].GetChecksum(),b,"var_202" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_239,b,"var_239");
		CrcCheck.ToByteArray(this.var_258,b,"var_258");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_325[a0][a1].GetChecksum(),b,"var_325" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_437,b,"var_437");
		CrcCheck.ToByteArray(this.var_447,b,"var_447");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
