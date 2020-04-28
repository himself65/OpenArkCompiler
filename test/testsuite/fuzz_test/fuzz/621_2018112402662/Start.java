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
public class Start
{
   	public static void main(String[] args)
   	{
   		System.out.printf("FIGO-FUZZ-START-FLAG\n");
	 	MainClass mainclass = new MainClass();
	 	mainclass.run();
	  	System.out.printf("FIGO-FUZZ-Checksum=0x%x\n" , mainclass.GetChecksum());
	  	Runtime.getRuntime().gc();
   	}
}

// DEPENDENCE: cl_4.java MainClass.java cl_30.java cl_35.java CRC32.java cl_41.java CrcCheck.java cl_37.java cl_7.java
// EXEC:%maple *.java %build_option -o %n.so
