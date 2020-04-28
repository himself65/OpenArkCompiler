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

//SEED=205973598

//import java.util.zip.CRC32;

class cl_109 
{
	   int [][][] var_90 = {{{(-1827119121)},{(-906018197)},{(-492526293)}},{{(567604741)},{(549838094)},{(381883604)}},{{(-7717794)},{(2004953713)},{(-258643)}},{{(-511901411)},{(-1792501838)},{(541747737)}},{{(49446871)},{(939234031)},{(-1295295670)}}};
	   int var_95 = (346077022);
	   short var_157 = (short)(4713);
	   byte var_234 = (byte)(-76);
	final   long var_238 = (-6338892837850530152L);
	   float var_334 = (9.13251e-15F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(79);
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_90[a0][a1][a2],b,"var_90" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_95,b,"var_95");
		CrcCheck.ToByteArray(this.var_157,b,"var_157");
		CrcCheck.ToByteArray(this.var_234,b,"var_234");
		CrcCheck.ToByteArray(this.var_238,b,"var_238");
		CrcCheck.ToByteArray(this.var_334,b,"var_334");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
