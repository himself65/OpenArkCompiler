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

import java.util.*;
import java.util.zip.CRC32;
import java.lang.Long;
import java.lang.Double;

class CrcBuffer{
	int i=0;
	static int buffer_counter = 0;
	byte [] buffer;
	CrcBuffer(int size){
		this.buffer = new byte[size];
		buffer_counter++;
		System.out.printf("Buffer-Counter=%d\tLength=%d\n",buffer_counter,size); 
	}
}

public class CrcCheck{
	/**************************************************************/
	public static void ToByteArray(byte args,CrcBuffer b,String var_name){
		b.buffer[b.i] = args;
		b.i++;
		//System.out.printf("Index=%d\tByte-Value=0x%x\n",b.i-1,args); 
		System.out.printf("Index=%d\tVarName=%s\tByte-Value=%d\n",b.i-1,var_name,args);
	}

	public static  void ToByteArray(short args,CrcBuffer b,String var_name){
		b.buffer[b.i] =(byte)((args>>8)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)(args&0xff);
		b.i++;
		//System.out.printf("Index=%d\tShort-Value=0x%x\n",b.i-2,args);  
		System.out.printf("Index=%d\tVarName=%s\tShort-Value=%d\n",b.i-2,var_name,args);
	}

	public static  void ToByteArray(int args,CrcBuffer b,String var_name){
		b.buffer[b.i] =(byte)((args>>24)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>16)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>8)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)(args&0xff);
		b.i++; 
		//System.out.printf("Index=%d\tInt-Value=0x%x\n",b.i-4,args); 
		System.out.printf("Index=%d\tVarName=%s\tInt-Value=%d\n",b.i-4,var_name,args);
	}

	
	public static  void ToByteArray(long args,CrcBuffer b,String var_name){
		b.buffer[b.i] =(byte)((args>>56)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>48)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>40)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>32)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>24)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>16)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((args>>8)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)(args&0xff);
		b.i++;
		//System.out.printf("Index=%d\tLong-Value=0x%x\n",b.i-8,args); 
		System.out.printf("Index=%d\tVarName=%s\tLong-Value=%d\n",b.i-8,var_name,args);
	}

	public static void ToByteArray(boolean args,CrcBuffer b,String var_name){
		if(args){
			b.buffer[b.i] = 0x01;
		}
		else{
			b.buffer[b.i] = 0x00;
		}
		b.i++;
		System.out.printf("Index=%d\tVarName=%s\tBoolean-Value=%d\n",b.i-1,var_name,args? 1 : 0);
	}

	public static  void ToByteArray(float args,CrcBuffer b,String var_name){
		int iargs=Float.floatToIntBits(args);
		b.buffer[b.i] =(byte)((iargs>>24)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((iargs>>16)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((iargs>>8)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)(iargs&0xff);
		b.i++; 
		//System.out.printf("Index=%d\tFloat-Value=0x%x\n",b.i-4,iargs);  
		System.out.printf("Index=%d\tVarName=%s\tFloat-Value=%f\n",b.i-4,var_name,args);
	}

	public static  void ToByteArray(double args,CrcBuffer b,String var_name){
		long largs=Double.doubleToLongBits(args);
		b.buffer[b.i] =(byte)((largs>>56)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((largs>>48)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((largs>>40)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((largs>>32)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((largs>>24)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((largs>>16)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)((largs>>8)&0xff);
		b.i++;
		b.buffer[b.i] =(byte)(largs&0xff);
		b.i++;
		//System.out.printf("Index=%d\tDouble-Value=0x%x\n",b.i-8,largs); 
		System.out.printf("Index=%d\tVarName=%s\tDouble-Value=%f\n",b.i-8,var_name,args);
	}
	

}
