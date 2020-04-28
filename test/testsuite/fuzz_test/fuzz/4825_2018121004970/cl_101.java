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

//SEED=2309790164

//import java.util.zip.CRC32;

class cl_101 
{
	   int var_113 = (-609594025);
	final   int [][][] var_153 = {{{(705408753),(-921736171)},{(-244864984),(-1141382535)},{(306789196),(1316276520)},{(512898319),(151586644)},{(189499308),(-401388611)}},{{(-1975834221),(-708488053)},{(573554818),(-735408544)},{(433092652),(1491552243)},{(3345465),(1574040000)},{(-1074018544),(-1248672986)}},{{(-1156286499),(-1826274011)},{(2016862984),(-810541301)},{(55449680),(-1325265670)},{(-1116700620),(-346394619)},{(-1318371227),(648105875)}},{{(639194003),(-1761746497)},{(1651221356),(1452725643)},{(-64842877),(1026607627)},{(1205836472),(984359607)},{(330123890),(-1447558144)}},{{(696016412),(-486397925)},{(-1111812907),(-657720150)},{(-70725177),(1624224493)},{(1247923136),(1318202070)},{(-1915641380),(1144539459)}},{{(1555363568),(280204677)},{(-88790895),(-1968392160)},{(1046664159),(-1963910722)},{(701479018),(169304127)},{(178935136),(-1136447879)}}};
	   double var_171 = (1.19187e-78D);
	   float var_235 = (-6.3488e+08F);
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(256);
		CrcCheck.ToByteArray(this.var_113,b,"var_113");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<5;++a1){
		for(int a2=0;a2<2;++a2){
			CrcCheck.ToByteArray(this.var_153[a0][a1][a2],b,"var_153" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_171,b,"var_171");
		CrcCheck.ToByteArray(this.var_235,b,"var_235");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
