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

//SEED=2025510452

//import java.util.zip.CRC32;

class cl_83 
{
	   double var_73 = (-1.24058e-140D);
	final   float var_80 = (4.38822e+33F);
	   long [] var_88 = {(-828744676719971772L),(5054517921124057953L),(-5114583130074993786L),(7004039984413565637L),(-4290014921351186276L),(-1366208666436720736L),(-5402918337827149695L)};
	   int var_109 = (1572428694);
	   short var_113 = (short)(-15349);
	   long var_122 = (-2214392377184977292L);
	   byte var_134 = (byte)(-39);
	   boolean var_242 = false;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(84);
		CrcCheck.ToByteArray(this.var_73,b,"var_73");
		CrcCheck.ToByteArray(this.var_80,b,"var_80");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_88[a0],b,"var_88" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_109,b,"var_109");
		CrcCheck.ToByteArray(this.var_113,b,"var_113");
		CrcCheck.ToByteArray(this.var_122,b,"var_122");
		CrcCheck.ToByteArray(this.var_134,b,"var_134");
		CrcCheck.ToByteArray(this.var_242,b,"var_242");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
