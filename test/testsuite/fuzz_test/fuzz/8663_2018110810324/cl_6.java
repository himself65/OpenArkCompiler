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

//SEED=545827268

import java.util.zip.CRC32;

class cl_6 
{
	   long var_59 = (4619763297513657057L);
	   short var_101 = (short)(-21714);
	   int var_102 = (-1284875276);
	   byte var_104 = (byte)(-14);
	   long [][] var_121 = {{(-1926383207983966243L),(-6718503052762371510L),(-3522248742971036479L),(3365958279222303935L)},{(539311577934015456L),(-7239793746616350139L),(5689565232681148718L),(-5024141572709695540L)},{(2269097422517700343L),(-4331091754641236005L),(7955472144786064683L),(-2788851509491639306L)},{(2937604953943932354L),(-5071700452404195144L),(-6234297556184183767L),(-9128433148977071722L)},{(7150608095172065613L),(-280175254356745833L),(2775802891883130517L),(-1768885420548423379L)},{(7984438400960949252L),(2571997025674319477L),(-4446249610910213449L),(4622669688066644886L)}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(207);
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_101,b,"var_101");
		CrcCheck.ToByteArray(this.var_102,b,"var_102");
		CrcCheck.ToByteArray(this.var_104,b,"var_104");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<4;++a1){
			CrcCheck.ToByteArray(this.var_121[a0][a1],b,"var_121" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
