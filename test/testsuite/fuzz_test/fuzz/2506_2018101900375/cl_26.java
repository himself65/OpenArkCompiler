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

//SEED=269244036

import java.util.zip.CRC32;

class cl_26 
{
	final   byte var_30 = (byte)(-120);
	   short var_37 = (short)(24233);
	   int [][] var_47 = {{(1603840696),(1456490443)},{(2083149855),(789882397)},{(-1804750724),(1021341584)},{(-510736007),(1282750059)}};
	   float var_55 = (1.68071e-35F);
	   boolean [] var_62 = {true,false,true,false,true,false,false};
	   long [][] var_68 = {{(-2028809866702470038L)},{(-1645672198866988633L)},{(-7897929245869028064L)},{(4844246590984277068L)},{(-6397138307180900803L)}};
	   boolean var_73 = true;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(87);
		CrcCheck.ToByteArray(this.var_30,b,"var_30");
		CrcCheck.ToByteArray(this.var_37,b,"var_37");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<2;++a1){
			CrcCheck.ToByteArray(this.var_47[a0][a1],b,"var_47" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_55,b,"var_55");
		for(int a0=0;a0<7;++a0){
			CrcCheck.ToByteArray(this.var_62[a0],b,"var_62" + "["+ Integer.toString(a0)+"]");
		}
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<1;++a1){
			CrcCheck.ToByteArray(this.var_68[a0][a1],b,"var_68" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_73,b,"var_73");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
