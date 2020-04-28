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

//SEED=189333068

//import java.util.zip.CRC32;

class cl_116 
{
	   long var_76 = (6516039371946602934L);
	   byte var_102 = (byte)(-65);
	   int var_160 = (1617940582);
	   boolean var_191 = true;
	   boolean [] var_231 = {true};
	   short var_248 = (short)(-26828);
	   int [][] var_265 = {{(-510737135),(-805316677)},{(1033001152),(1546727080)}};
	   double [][][] var_296 = {{{(5.727e-103D),(-7.21109e+129D),(7.0347e-155D),(1.96148e-112D),(8.01109e-256D),(-17.3913D)},{(-4.63116e+288D),(2.71494e-33D),(1.08853e+108D),(1.09558e+221D),(3.57704e+160D),(-85.1344D)},{(-4.92159e+277D),(-9.26299e+167D),(1.06257e-113D),(1.03768e+147D),(1.17023e-174D),(-2.53888e+280D)},{(8.88937e-284D),(-4.3982e+269D),(4.35407e-193D),(-2.57654e-87D),(1.31214e+65D),(5.10544e+110D)}},{{(-1.64032e-136D),(-1.33718e+22D),(-1.71117e+40D),(5.13393e+245D),(4.32362e+133D),(4.03698e+227D)},{(-1.75494e+216D),(2.73917e-222D),(-3.73846e-167D),(-4.73034e-280D),(-1.71744e-262D),(-3.91886e-25D)},{(9.96782e+210D),(-1.33657e-247D),(3.25711e-296D),(2.11132e-143D),(5.05234e-240D),(-5.37438e-100D)},{(-4.34666e-155D),(2.17398e+91D),(3.03837e-22D),(-29.2164D),(-1.45791e+178D),(2.23593e+280D)}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(417);
		CrcCheck.ToByteArray(this.var_76,b,"var_76");
		CrcCheck.ToByteArray(this.var_102,b,"var_102");
		CrcCheck.ToByteArray(this.var_160,b,"var_160");
		CrcCheck.ToByteArray(this.var_191,b,"var_191");
		for(int a0=0;a0<1;++a0){
			CrcCheck.ToByteArray(this.var_231[a0],b,"var_231" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_248,b,"var_248");
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_265[a0][a1],b,"var_265" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<2;++a0){
		for(int a1=0;a1<4;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_296[a0][a1][a2],b,"var_296" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
