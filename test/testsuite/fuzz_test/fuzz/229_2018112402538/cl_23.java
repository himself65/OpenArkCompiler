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

//SEED=59004558

//import java.util.zip.CRC32;

class cl_23 
{
	   float var_97 = (-1.41519e+29F);
	   boolean var_110 = true;
	   int var_120 = (381318680);
	   short var_154 = (short)(19483);
	   double var_195 = (-1.16533e-296D);
	final   byte var_233 = (byte)(-89);
	   int [][] var_300 = {{(-768528273),(-1117099593),(-1127910748),(1480134932),(1946405336)},{(1827824104),(-1142640154),(1342911672),(963929065),(1652496074)},{(-348755192),(-1233176238),(-247404493),(809598811),(-1142769188)},{(-1716011065),(-956827127),(894099873),(-961797425),(871587079)},{(-1647748050),(134909836),(-1839835876),(-1102058722),(-117238325)},{(2118462836),(-1956531984),(-1638731434),(-1997224947),(257936025)}};
	final   long var_539 = (-1046463752620615186L);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(148);
		CrcCheck.ToByteArray(this.var_97,b,"var_97");
		CrcCheck.ToByteArray(this.var_110,b,"var_110");
		CrcCheck.ToByteArray(this.var_120,b,"var_120");
		CrcCheck.ToByteArray(this.var_154,b,"var_154");
		CrcCheck.ToByteArray(this.var_195,b,"var_195");
		CrcCheck.ToByteArray(this.var_233,b,"var_233");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
			CrcCheck.ToByteArray(this.var_300[a0][a1],b,"var_300" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_539,b,"var_539");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
