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

class cl_76 
{
	final   double [][] var_83 = {{(-2.11346e+62D),(-6.71608e+277D),(-1.10554e+15D),(-2.39071e+251D),(4.99166e+29D)}};
	   int [] var_89 = {(2044003010),(840966728),(-213211832),(351948512),(477271701)};
	   int [][] var_107 = {{(-2049300974),(391809841),(396465155),(-432133906),(-722367138),(1087402515),(-144260173)},{(1642985647),(1344358480),(1275281940),(802376216),(-1721430619),(-1553987845),(-666714824)},{(-1976525269),(1288222157),(-1353508106),(-456731262),(1937509897),(-781486788),(1460779941)},{(-1247996821),(-290843795),(1347247921),(-611797461),(305211111),(-55215344),(-52666297)},{(1018105779),(776737638),(831698496),(1739281895),(1475205026),(1180266936),(-1736734178)},{(1622432858),(-1350200804),(942624357),(1096880757),(-2024017177),(1161592647),(-1807939423)}};
	   boolean var_114 = true;
	   boolean [][][] var_123 = {{{true,false,true,false,false,true},{true,false,true,true,true,false},{true,true,true,true,false,false},{false,false,true,true,true,false}},{{true,true,false,false,true,true},{true,false,false,false,true,false},{false,true,true,true,false,false},{true,true,true,false,false,true}},{{true,true,false,false,true,false},{true,false,false,false,false,true},{true,true,false,false,false,false},{true,false,false,false,false,true}}};
	   short var_147 = (short)(12990);
	   double var_211 = (3.5665e+193D);
	final   byte var_213 = (byte)(69);
	   int var_248 = (-2036162129);
	   double [] var_265 = {(1.37852e-119D),(7.48195e+27D),(-3.41179e+305D),(-3.61286e-26D),(8.28396e+187D),(-1.76685e+246D),(-5.53813e+218D)};
	   long [] var_269 = {(-1115114709225623499L)};
	final   int [][][] var_292 = {{{(-1759121878),(-550070032),(1715542814),(944069059)},{(1842947169),(-1883009977),(-413394645),(-1002169742)}},{{(709003447),(-2138539485),(-224170741),(1996897380)},{(-663753197),(-104249727),(-1493972988),(28770248)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(444);
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_83[a0][a1],b,"var_83" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<5;++a0){
			CrcCheck.ToByteArray(this.var_89[a0],b,"var_89" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_107[a0][a1],b,"var_107" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_123[a0][a1][a2],b,"var_123" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_147,b,"var_147");
		CrcCheck.ToByteArray(this.var_211,b,"var_211");
		CrcCheck.ToByteArray(this.var_213,b,"var_213");
		CrcCheck.ToByteArray(this.var_248,b,"var_248");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_265[a0],b,"var_265" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_269[a0],b,"var_269" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_292[a0][a1][a2],b,"var_292" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
