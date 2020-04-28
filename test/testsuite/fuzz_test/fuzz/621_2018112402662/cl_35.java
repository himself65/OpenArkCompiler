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

//SEED=1902118766

//import java.util.zip.CRC32;

class cl_35 extends cl_7
{
	   cl_7 [][][] var_60 = {{{new cl_7()},{new cl_7()}},{{new cl_7()},{new cl_7()}},{{new cl_7()},{new cl_7()}}};
	   cl_7 var_62 = new cl_7();
	   cl_4 var_83 = new cl_4();
	   cl_30 var_87 = new cl_30();
	   boolean [][][] var_93 = {{{false,false,true,false,true,false},{true,true,false,true,false,true},{false,true,true,true,true,true},{false,false,false,true,false,true},{true,true,true,false,true,false},{true,false,false,true,true,false},{true,true,false,true,false,false}},{{false,false,true,false,false,true},{false,false,false,false,true,false},{true,true,true,true,true,false},{true,false,true,true,true,true},{true,true,true,true,true,true},{true,false,true,true,true,true},{true,false,false,true,false,false}},{{false,true,true,true,false,false},{false,false,false,true,false,false},{true,true,true,true,false,false},{false,false,true,true,false,false},{false,true,true,true,false,false},{true,false,true,false,false,true},{false,false,true,false,false,true}},{{false,false,true,true,true,false},{true,true,false,false,true,true},{true,true,false,false,false,false},{false,false,false,false,true,false},{true,true,true,true,true,true},{false,false,true,false,false,true},{true,true,false,true,false,true}}};
	   cl_4 [][][] var_157 = {{{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()}},{{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()}},{{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()}},{{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()}},{{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()}},{{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()}},{{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()},{new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4(),new cl_4()}}};
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(2806);
		CrcCheck.ToByteArray(this.var_11,b,"var_11");
		CrcCheck.ToByteArray(this.var_13,b,"var_13");
		CrcCheck.ToByteArray(this.var_29,b,"var_29");
		for(int a0=0;a0<3;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<1;++a2){
			CrcCheck.ToByteArray(this.var_60[a0][a1][a2].GetChecksum(),b,"var_60" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_62.GetChecksum(),b,"var_62.GetChecksum()");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_63[a0][a1],b,"var_63" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_83.GetChecksum(),b,"var_83.GetChecksum()");
		CrcCheck.ToByteArray(this.var_87.GetChecksum(),b,"var_87.GetChecksum()");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_93[a0][a1][a2],b,"var_93" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_97[a0][a1][a2],b,"var_97" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_98,b,"var_98");
		for(int a0=0;a0<7;++a0){
		for(int a1=0;a1<7;++a1){
		for(int a2=0;a2<6;++a2){
			CrcCheck.ToByteArray(this.var_157[a0][a1][a2].GetChecksum(),b,"var_157" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]"+".GetChecksum()");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_217,b,"var_217");
		CrcCheck.ToByteArray(this.var_283,b,"var_283");
		CrcCheck.ToByteArray(this.var_296,b,"var_296");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
