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

//SEED=764790408

//import java.util.zip.CRC32;

class cl_65 
{
	   double var_52 = (6.56791e-16D);
	   float var_58 = (8.24876e+37F);
	   byte var_59 = (byte)(-120);
	   int var_75 = (-398448237);
	final   short var_82 = (short)(2015);
	   int [][][] var_101 = {{{(-1009320809),(789466479),(122201560),(1569245340)},{(-1946580350),(-507053990),(-1415218566),(1627446122)},{(477040524),(-88652061),(1839801928),(44401650)}},{{(-1168058847),(109206480),(-1673971454),(-857826916)},{(1183686990),(-1846641398),(-1437691092),(84934419)},{(-1208551795),(240260210),(-521802344),(-1773308815)}},{{(-1360164006),(-1039116439),(-756095491),(1643931736)},{(551943944),(-1954482843),(-1792708012),(330608525)},{(-773374921),(1099082495),(-1746246942),(643940855)}},{{(-659684293),(1251689454),(-1483452039),(152542833)},{(1531550856),(2084116257),(-502037883),(743429781)},{(-1647981446),(1955895094),(-1996828685),(-84421722)}},{{(-804204051),(2045552119),(9949939),(2080788178)},{(-392976210),(1883652329),(-909136343),(576235016)},{(592016601),(1393349967),(771038895),(-747677577)}},{{(2001135334),(-355863645),(1517622312),(-1735946337)},{(-1683625520),(-1152219932),(300110118),(315286357)},{(1498659769),(-1709601514),(-2136962700),(-802493410)}}};
	   long var_134 = (5017151865002531238L);
	   boolean var_160 = false;
/*********************************/

	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(316);
		CrcCheck.ToByteArray(this.var_52,b,"var_52");
		CrcCheck.ToByteArray(this.var_58,b,"var_58");
		CrcCheck.ToByteArray(this.var_59,b,"var_59");
		CrcCheck.ToByteArray(this.var_75,b,"var_75");
		CrcCheck.ToByteArray(this.var_82,b,"var_82");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<3;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_101[a0][a1][a2],b,"var_101" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_134,b,"var_134");
		CrcCheck.ToByteArray(this.var_160,b,"var_160");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
