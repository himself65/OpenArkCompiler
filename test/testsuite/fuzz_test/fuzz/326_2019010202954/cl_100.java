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

//SEED=538048686

//import java.util.zip.CRC32;

class cl_100 
{
	   double var_70 = (8.81385e-111D);
	   long var_77 = (4389403111596022123L);
	final   long [][] var_88 = {{(343010707749934819L),(-7067304214456653645L),(-1720946528573572274L),(-6827068922486482030L),(4581128704408823737L)},{(-8944672306847716139L),(-2455023433652486370L),(1725731083383904110L),(-1761666139651480650L),(1622601915734113184L)},{(-161311910856815767L),(-2899780519714832605L),(2344565802313404058L),(-3586432854774312295L),(3872659907668983256L)},{(-4463892588833262447L),(3577258062525383534L),(7948056784735094731L),(-3541318981839720934L),(7082492804530590404L)},{(1047073401151268100L),(798901914483810360L),(-430665683575483802L),(-1482719779938774353L),(-8225330596588823680L)},{(8727301324922518055L),(-5835039583164939257L),(6096737232778019680L),(-2377127083666566737L),(5803680893143673139L)},{(2403981377041917310L),(-3296748839976403847L),(-5311241514547696117L),(-5570701433002801868L),(-7346718456315025061L)}};
	   float var_103 = (-0.00513271F);
	   int var_110 = (499803681);
	   byte var_113 = (byte)(-124);
	final   short var_114 = (short)(22743);
	   boolean var_146 = false;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(308);
		CrcCheck.ToByteArray(this.var_70,b,"var_70");
		CrcCheck.ToByteArray(this.var_77,b,"var_77");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_88[a0][a1],b,"var_88" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_103,b,"var_103");
		CrcCheck.ToByteArray(this.var_110,b,"var_110");
		CrcCheck.ToByteArray(this.var_113,b,"var_113");
		CrcCheck.ToByteArray(this.var_114,b,"var_114");
		CrcCheck.ToByteArray(this.var_146,b,"var_146");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
