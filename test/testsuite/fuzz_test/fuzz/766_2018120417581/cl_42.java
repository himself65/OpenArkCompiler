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

class cl_42 
{
	   float var_55 = (0.00875881F);
	   byte var_65 = (byte)(20);
	   boolean var_72 = true;
	   int var_74 = (113637025);
	   double [][] var_91 = {{(-1.84772e+303D),(-0.0136294D),(1.2429e-253D),(-5.35701e-298D),(-9.58933e-71D)}};
	   int [][] var_104 = {{(867829029),(1347184668),(-1000980066),(785583484),(-1164691880),(-2058644283),(-607751749)},{(1689406258),(-1196277350),(-2072090472),(-1924291805),(127676617),(-690849616),(-1687436641)},{(1262112410),(-1563026241),(860122368),(-1694058693),(1715983498),(-1742692168),(-1153883694)},{(576398646),(-236469592),(132359602),(12545199),(1248101735),(-2034438885),(-1372782488)},{(-665253746),(-982894444),(-1816799150),(2136611606),(-2067298822),(-5060743),(-1984238287)},{(1480821032),(-636888972),(435113421),(-2009754530),(1018990697),(1432775870),(1902026019)}};
	final   short var_122 = (short)(-10619);
	final   int [][][] var_289 = {{{(272319218),(400412239)},{(-209766554),(-848648018)},{(-429445965),(1906293632)},{(-315979939),(1257576266)},{(989106170),(-1338008966)},{(-1601771887),(-1846332685)}},{{(-1251708197),(-1831211667)},{(1253934548),(-1116644236)},{(-129835688),(-1793379924)},{(-941149600),(1571910387)},{(-508926485),(-129332165)},{(-1367098227),(-1873105237)}}};
	   double var_296 = (2.51095e-13D);
	final   long var_441 = (1432189517947594669L);
	   long var_458 = (2497669026784793054L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(340);
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		CrcCheck.ToByteArray(this.var_65,b,"var_65");
		CrcCheck.ToByteArray(this.var_72,b,"var_72");
		CrcCheck.ToByteArray(this.var_74,b,"var_74");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_91[a0][a1],b,"var_91" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_104[a0][a1],b,"var_104" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_122,b,"var_122");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_289[a0][a1][a2],b,"var_289" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_296,b,"var_296");
		CrcCheck.ToByteArray(this.var_441,b,"var_441");
		CrcCheck.ToByteArray(this.var_458,b,"var_458");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
