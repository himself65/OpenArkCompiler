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

//SEED=1973468952

//import java.util.zip.CRC32;

class cl_44 
{
	   cl_52 var_63 = new cl_52();
	final   cl_21 var_73 = new cl_21();
	   cl_21 var_98 = new cl_21();
	   cl_60 var_111 = new cl_60();
	   cl_86 var_117 = new cl_86();
	   int var_125 = (-1112642460);
	   float [] var_145 = {(3.45477e+06F)};
	final   short var_165 = (short)(26053);
	   double var_182 = (1.41043e-59D);
	   boolean var_200 = false;
	   int [][][] var_320 = {{{(1146951524),(970851104),(-2111283883),(1032009598),(2057681400),(1310720765)},{(764632549),(-1660577925),(1362421412),(1186228066),(-641787049),(-1266206838)},{(-600530385),(-1229532068),(962003583),(-104509574),(1126667755),(-1937964920)},{(-63369023),(-927947397),(1173088196),(541590792),(2024416125),(1620059296)},{(470967908),(-50168398),(-957753714),(-2068692506),(-1196445834),(-570835853)},{(-374756653),(-887462527),(2012955650),(-460625746),(-2079780426),(1099660514)}},{{(-1401462383),(1904461099),(880088088),(1284337836),(-1072706926),(1313769092)},{(-1945212599),(1574962677),(-99076334),(-1431619933),(-347789169),(-372713451)},{(-51166255),(-1733551348),(1332191179),(1794676000),(1691704038),(-1074448574)},{(-1581743446),(465988753),(1005502678),(-483862968),(-1005847943),(273711518)},{(-452945154),(704156311),(1625347591),(1822451342),(-1784440228),(698645447)},{(531429766),(-1031286715),(279688992),(975268347),(-232579281),(-248232906)}}};
	   float var_328 = (-3.7848e-31F);
	   long var_385 = (-1575274938517647051L);
	   byte var_435 = (byte)(-107);
	   int [] var_503 = {(-1503118831),(1344694380),(874375032),(-8055848),(772488644)};
	   double [][] var_597 = {{(7.30901e-225D),(4.4331e+95D),(4.7355e+71D),(1.01778e+14D),(-8.25712e+252D)},{(1.47761e-217D),(-3.75773e+36D),(5.19922e+151D),(-4.38629e+264D),(8.73584e-57D)},{(-5.79118e+78D),(1.08474e-168D),(-3.86659e-168D),(-8.84309e-225D),(-1.88691e+129D)},{(3.86369e-50D),(7.68236e-201D),(-2.21119e-281D),(5.10392e+73D),(1.49227e+23D)},{(-1.37751e-79D),(-5.12027e-191D),(9.0578e+23D),(-3.69272e-207D),(1.17809e-08D)},{(3.11083e+14D),(1.7864e-16D),(9.22581e-250D),(6.87716e+257D),(1.80527e+27D)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(620);
		CrcCheck.ToByteArray(this.var_63.GetChecksum(),b,"var_63.GetChecksum()");
		CrcCheck.ToByteArray(this.var_73.GetChecksum(),b,"var_73.GetChecksum()");
		CrcCheck.ToByteArray(this.var_98.GetChecksum(),b,"var_98.GetChecksum()");
		CrcCheck.ToByteArray(this.var_111.GetChecksum(),b,"var_111.GetChecksum()");
		CrcCheck.ToByteArray(this.var_117.GetChecksum(),b,"var_117.GetChecksum()");
		CrcCheck.ToByteArray(this.var_125,b,"var_125");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_145[a0],b,"var_145" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_165,b,"var_165");
		CrcCheck.ToByteArray(this.var_182,b,"var_182");
		CrcCheck.ToByteArray(this.var_200,b,"var_200");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_320[a0][a1][a2],b,"var_320" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_328,b,"var_328");
		CrcCheck.ToByteArray(this.var_385,b,"var_385");
		CrcCheck.ToByteArray(this.var_435,b,"var_435");
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_503[a0],b,"var_503" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_597[a0][a1],b,"var_597" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
