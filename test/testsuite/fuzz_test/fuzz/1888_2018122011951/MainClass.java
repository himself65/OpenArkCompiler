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

//SEED=1925474860
//import java.util.zip.CRC32;
class MainClass 
{
	final  static float var_6 = (2.5367e+27F);
	final  static boolean [] var_10 = {false,true,false,false,true,true};
	final   long var_22 = (1012611220370082784L);
	  static float [][][] var_31 = {{{(1.62427e+07F),(6.35645e-25F),(6.99256e-11F),(2.48448e-13F),(-6.15252e-27F),(-1.29425e+24F),(-7.32e-07F)},{(-4.495e+30F),(1.10025e-29F),(1.63596e-08F),(-7.89483e+06F),(-1.80485e-12F),(-1.39264e+24F),(3.02761e-25F)},{(-6.40164e+18F),(-0.00151572F),(-1.79265e-31F),(8.53271e+09F),(4.69127e-08F),(2.31831e+18F),(-1.87447e-28F)},{(-1.50292e+38F),(4.94129e+21F),(8700.33F),(7995.76F),(6.95455e+13F),(-3.73369e-10F),(-4.35374e+32F)},{(1.85029e-36F),(1.6326F),(-5.85116e-21F),(-0.000712358F),(9.82595e+25F),(-0.00298436F),(-1.08659e-16F)},{(-2.68117e-13F),(-3.78597e-32F),(-6.32703e-19F),(1.24339e-20F),(2.34053e-05F),(-3.43505e-38F),(-0.000330687F)}},{{(-9.61096e+12F),(-1.17381e+32F),(6.15862e+07F),(-5.02787e-11F),(9.0435e-26F),(2.82434e+20F),(2.6241e+35F)},{(72.1152F),(6.9146e+14F),(5.16817e-15F),(-6.18514e+20F),(-8.94485e-05F),(-1.89303e+09F),(-5.32361e+10F)},{(-1.11853e+19F),(-1.10491e-37F),(-1.9491e+12F),(-26.8091F),(-9.85732e-15F),(-3.01214e+32F),(-1.04591e+19F)},{(-3.60373e+28F),(5.5892e+12F),(0.051311F),(-5.94988e+15F),(2.5191e+11F),(-0.00478722F),(2.78452e+35F)},{(3.21115e+08F),(-4.67735e+24F),(-3.26124e+21F),(4.81564e-18F),(7.52748e-09F),(7.26904e+37F),(9.89825e+33F)},{(1.04479e-07F),(-1.6377e+23F),(-5.5914e+14F),(21.7393F),(7.30426e-29F),(3.81414e-16F),(7.38719e-29F)}},{{(-1.23145e+17F),(6.05103e+33F),(1.73953e-14F),(-2.59492e-06F),(-5.89452e+27F),(-1.15173e+09F),(-1.19835e+23F)},{(8.22344e-08F),(-2.14873e-33F),(0.328485F),(7.9058e+07F),(-4.90948e+18F),(9.97536e+30F),(2.95974e-33F)},{(7.22176e+36F),(8.10192e-29F),(227.029F),(-5.6305e-11F),(-1.56078e-34F),(2.1394e-17F),(8.40658e-10F)},{(-2.57922e+07F),(-4.02681e-37F),(1.994e+34F),(0.0620787F),(1.25845e-32F),(0.00921665F),(-5.65844e-09F)},{(6.20487e-37F),(6.22692e-28F),(7.97428e-38F),(1.02671e-16F),(1.89435e+09F),(-1.74022e+36F),(-1.88538e+26F)},{(-1.69092e+34F),(-1.35624e+21F),(4.24542e-09F),(383.647F),(-1.14676e-33F),(2.1982e-11F),(-1.51397e+29F)}},{{(-29.3565F),(8.35293e-35F),(5.8172e+09F),(2.05282e+29F),(-8.27544e-37F),(3.08868e+09F),(-85316.6F)},{(-1.3423e+11F),(-0.00178527F),(-1.02908e-16F),(2.43295e-26F),(-0.00133567F),(-0.231081F),(-6.5192e-37F)},{(-2.86938e+33F),(-7.67746e+06F),(-7.11177e+16F),(-2.25033e-26F),(14797F),(-1.34711e-14F),(4.8956e+31F)},{(-1.21589e+38F),(-1.34692e+09F),(4.15701e+06F),(4.69559e-32F),(-2.85931e+08F),(-8.82243e+31F),(-7.40889e+36F)},{(-2.01503e-26F),(-49.6619F),(-5.17262e+08F),(-6.09846e+31F),(5.6192e+18F),(-1.92145e-28F),(-0.00640101F)},{(-7.388e+28F),(9.11198e-29F),(-3.93602e-24F),(-1.07402e+27F),(0.0786557F),(4.88335e+30F),(-5.0723e-16F)}},{{(1.42586e-20F),(-5.99139e+25F),(4.22567e-12F),(-1.15004e+18F),(-2.42117e-19F),(-8.78637e-27F),(-2.56649e+34F)},{(1.15226e-27F),(1.00885e+30F),(42866.5F),(0.0724541F),(3.48506e+33F),(-2.27464e-35F),(3.88824e-28F)},{(3.09021e+07F),(3.02526e-07F),(-1.4945e+17F),(5.80766e-33F),(-2.82488e-22F),(0.105227F),(1.18603e+34F)},{(2.11164e+12F),(2.01992e+24F),(-1.20029e+36F),(-2.49446e-15F),(-0.000109573F),(-6.24017e-09F),(1.97507e+13F)},{(-6.15519e-12F),(7.81295e+21F),(2.68574e-26F),(-7.96569e-08F),(3.43809e+22F),(-4.60739e-38F),(4.40948e-13F)},{(1.99703e-37F),(6851.34F),(2.01886e-31F),(5.12373e+27F),(-19.3475F),(-9.90335e+37F),(1.28766e+14F)}}};
	  static cl_26 var_33 = new cl_26();
	  static byte var_35 = (byte)(73);
	  static long var_36 = (-3795955194745945992L);
	  static long var_37 = (-1649178432347680365L);
	  static boolean [][][] var_38 = {{{false,true,true,false},{true,true,true,true}}};
	  static int var_49 = (994583430);
	  static cl_3 var_128 = new cl_3();
	final  static cl_95 var_129 = new cl_95();
	  static cl_5 var_131 = new cl_5();
	  static int var_133 = (-1412280985);
	  static cl_29 var_135 = new cl_29();
	  static byte var_143 = (byte)(20);
	  static int var_145 = (1511052544);
	final  static cl_108 var_148 = new cl_108();
	  static short var_151 = (short)(-10639);
	  static long var_158 = (-1459345746000245487L);
	   byte var_160 = (byte)(-128);
	   int var_168 = (-1984254659);
	   int var_172 = (1798974569);
	   cl_95 var_173 = new cl_95();
	   short var_183 = (short)(27572);
	   short var_185 = (short)(-2029);
	   short var_186 = (short)(-17462);
	   int var_187 = (-1679856495);
	   short var_198 = (short)(19008);
	   int var_201 = (-243247371);
	   boolean var_206 = false;
	   cl_26 [][] var_207 = {{new cl_26(),new cl_26(),new cl_26()},{new cl_26(),new cl_26(),new cl_26()},{new cl_26(),new cl_26(),new cl_26()},{new cl_26(),new cl_26(),new cl_26()},{new cl_26(),new cl_26(),new cl_26()},{new cl_26(),new cl_26(),new cl_26()}};
	   long var_215 = (-3210757676290494909L);
	   boolean [][] var_219 = {{true,true,false,false,false,true,false},{true,false,true,true,false,true,true},{false,true,true,false,true,false,false},{true,false,true,true,true,true,false}};
	   byte var_220 = (byte)(87);
	   int var_221 = (1698249812);
/*********************************/
public strictfp void run()

{	var_49 =var_128.func_53(var_129 ,false ,false);
	//var_38[0][1][var_49] = (!(!(((--var_35)-(short)(29028))==var_135.var_136))) ;

}
	public  long GetChecksum()
	{
		System.out.printf("---Begin GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		CrcBuffer b = new CrcBuffer(1173);
		CrcCheck.ToByteArray(this.var_6,b,"var_6");
		for(int a0=0;a0<6;++a0){
			CrcCheck.ToByteArray(this.var_10[a0],b,"var_10" + "["+ Integer.toString(a0)+"]");
		}
		CrcCheck.ToByteArray(this.var_22,b,"var_22");
		for(int a0=0;a0<5;++a0){
		for(int a1=0;a1<6;++a1){
		for(int a2=0;a2<7;++a2){
			CrcCheck.ToByteArray(this.var_31[a0][a1][a2],b,"var_31" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_33.GetChecksum(),b,"var_33.GetChecksum()");
		CrcCheck.ToByteArray(this.var_35,b,"var_35");
		CrcCheck.ToByteArray(this.var_36,b,"var_36");
		CrcCheck.ToByteArray(this.var_37,b,"var_37");
		for(int a0=0;a0<1;++a0){
		for(int a1=0;a1<2;++a1){
		for(int a2=0;a2<4;++a2){
			CrcCheck.ToByteArray(this.var_38[a0][a1][a2],b,"var_38" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]" + "["+ Integer.toString(a2)+"]");
		}
		}
		}
		CrcCheck.ToByteArray(this.var_49,b,"var_49");
		CrcCheck.ToByteArray(this.var_128.GetChecksum(),b,"var_128.GetChecksum()");
		CrcCheck.ToByteArray(this.var_129.GetChecksum(),b,"var_129.GetChecksum()");
		CrcCheck.ToByteArray(this.var_131.GetChecksum(),b,"var_131.GetChecksum()");
		CrcCheck.ToByteArray(this.var_133,b,"var_133");
		CrcCheck.ToByteArray(this.var_135.GetChecksum(),b,"var_135.GetChecksum()");
		CrcCheck.ToByteArray(this.var_143,b,"var_143");
		CrcCheck.ToByteArray(this.var_145,b,"var_145");
		CrcCheck.ToByteArray(this.var_148.GetChecksum(),b,"var_148.GetChecksum()");
		CrcCheck.ToByteArray(this.var_151,b,"var_151");
		CrcCheck.ToByteArray(this.var_158,b,"var_158");
		CrcCheck.ToByteArray(this.var_160,b,"var_160");
		CrcCheck.ToByteArray(this.var_168,b,"var_168");
		CrcCheck.ToByteArray(this.var_172,b,"var_172");
		CrcCheck.ToByteArray(this.var_173.GetChecksum(),b,"var_173.GetChecksum()");
		CrcCheck.ToByteArray(this.var_183,b,"var_183");
		CrcCheck.ToByteArray(this.var_185,b,"var_185");
		CrcCheck.ToByteArray(this.var_186,b,"var_186");
		CrcCheck.ToByteArray(this.var_187,b,"var_187");
		CrcCheck.ToByteArray(this.var_198,b,"var_198");
		CrcCheck.ToByteArray(this.var_201,b,"var_201");
		CrcCheck.ToByteArray(this.var_206,b,"var_206");
		for(int a0=0;a0<6;++a0){
		for(int a1=0;a1<3;++a1){
			CrcCheck.ToByteArray(this.var_207[a0][a1].GetChecksum(),b,"var_207" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]"+".GetChecksum()");
		}
		}
		CrcCheck.ToByteArray(this.var_215,b,"var_215");
		for(int a0=0;a0<4;++a0){
		for(int a1=0;a1<7;++a1){
			CrcCheck.ToByteArray(this.var_219[a0][a1],b,"var_219" + "["+ Integer.toString(a0)+"]" + "["+ Integer.toString(a1)+"]");
		}
		}
		CrcCheck.ToByteArray(this.var_220,b,"var_220");
		CrcCheck.ToByteArray(this.var_221,b,"var_221");
		CRC32 c = new CRC32();
		c.update(b.buffer,0,b.i);
		System.out.printf("---End GetChecksum:%s\tLine=%d\n",Thread.currentThread().getStackTrace()[1].getClassName(),Thread.currentThread().getStackTrace()[1].getLineNumber());
		return c.getValue();
	}
}
