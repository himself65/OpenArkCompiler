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

//SEED=3798900212

import java.util.zip.CRC32;

class cl_6 
{
	   long var_26 = (-1629736733382040440L);
	   boolean var_29 = true;
	   int [][][] var_37 = {{{(1947132671),(541607880),(943431201),(-1066935381),(-1360057386),(-1265980655)}},{{(758153815),(-641316439),(-1639850109),(-986272874),(-1774835498),(-261727412)}},{{(-651070408),(832375677),(1562998272),(1014336062),(-608694958),(1465221741)}}};
	   int var_43 = (614933668);
	   short var_78 = (short)(6654);
	   float var_81 = (0.000348065F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(91);
		CrcCheck.ToByteArray(this.var_26,b,"var_26");
		CrcCheck.ToByteArray(this.var_29,b,"var_29");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<1;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_37[a0][a1][a2],b,"var_37" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_43,b,"var_43");
		CrcCheck.ToByteArray(this.var_78,b,"var_78");
		CrcCheck.ToByteArray(this.var_81,b,"var_81");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
