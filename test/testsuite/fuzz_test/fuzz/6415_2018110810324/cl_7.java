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

//SEED=598886972

import java.util.zip.CRC32;

class cl_7 
{
	   double var_63 = (-3.2622e+297D);
	   float var_64 = (3.28637e+35F);
	   int var_65 = (201274198);
	   double [][] var_68 = {{(-3.05417e+221D),(-4.23327e-95D),(-1.39699e-292D)},{(8.74527e+20D),(5.956e+176D),(-1.78067e-298D)},{(-1.72637e-241D),(9.6288e-198D),(-1.74996e-62D)},{(-5.83756e+170D),(-5.48975e-58D),(-1.43195e+29D)}};
	   boolean [] var_71 = {true,false,false,false,true,true,true};
	   int [] var_79 = {(2102452022),(-1619936519),(-141298168),(-21412265),(597187722),(1774728403)};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(143);
		CrcCheck.ToByteArray(this.var_63,b,"var_63");
		CrcCheck.ToByteArray(this.var_64,b,"var_64");
		CrcCheck.ToByteArray(this.var_65,b,"var_65");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_68[a0][a1],b,"var_68" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_71[a0],b,"var_71" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<6;++a0){
			CrcCheck.ToByteArray(this.var_79[a0],b,"var_79" + "["+ Integer.toString(a0)+"]");
		}
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
