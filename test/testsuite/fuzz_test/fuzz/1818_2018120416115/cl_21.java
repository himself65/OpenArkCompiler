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

class cl_21 
{
	   long var_59 = (-4374578691142925598L);
	   float [] var_74 = {(3.03071F)};
	   byte var_81 = (byte)(73);
	   float var_89 = (-2.20499e-28F);
	   int var_91 = (-1799223085);
	   boolean var_107 = false;
	   short var_112 = (short)(30566);
	   double var_136 = (5.82351e-177D);
	   double [][] var_268 = {{(-7.6995e-199D),(-2.12644e+98D),(3.02904e+291D),(-4.23408e-228D),(8.82205e+222D)},{(3.64424e-53D),(-6.50544e-136D),(2.01314e-248D),(3.537e-22D),(3.01442e-197D)},{(8.41748e-101D),(-1.45849e+56D),(-6.24464e+53D),(1.68355e-96D),(-3.31663e+181D)},{(2.89114e+254D),(-7.36419e-289D),(2.94578e+82D),(3.98234e+155D),(-7.23607e-07D)},{(-5.07803e+258D),(-5.16966e+23D),(3.33021e-28D),(-6.504e-85D),(-2.36267e+52D)},{(1.9861e+153D),(-3.0644e-234D),(-1.09915e+291D),(-8.06519e-112D),(8.08994e-286D)}};
	   boolean [][][] var_433 = {{{true,false,true,false,false,true,false},{true,true,true,false,true,false,false}},{{false,false,false,false,false,true,false},{true,false,false,true,false,true,false}},{{true,false,false,false,true,false,false},{false,true,true,false,false,false,false}},{{false,false,false,true,false,true,false},{false,true,true,true,false,false,false}},{{true,false,false,true,false,false,false},{true,true,true,true,false,false,true}},{{false,false,true,false,false,true,true},{false,true,false,false,true,true,false}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(356);
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_74[a0],b,"var_74" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CrcCheck.ToByteArray(this.var_89,b,"var_89");
		CrcCheck.ToByteArray(this.var_91,b,"var_91");
		CrcCheck.ToByteArray(this.var_107,b,"var_107");
		CrcCheck.ToByteArray(this.var_112,b,"var_112");
		CrcCheck.ToByteArray(this.var_136,b,"var_136");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_268[a0][a1],b,"var_268" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_433[a0][a1][a2],b,"var_433" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
