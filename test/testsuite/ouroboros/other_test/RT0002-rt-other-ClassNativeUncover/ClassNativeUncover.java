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
 * -@TestCaseID: natives/ClassNativeUncover.java
 * -@Title/Destination: ClassNativeUncover Methods
 * -@Brief:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClassNativeUncover.java
 * -@ExecuteClass: ClassNativeUncover
 * -@ExecuteArgs:
 */

import java.lang.annotation.Annotation;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Type;
import java.lang.reflect.TypeVariable;

public class ClassNativeUncover {
    private static int res = 99;
    public String name;

    public ClassNativeUncover() {
        super();

    }

    public ClassNativeUncover(String name) {
        super();
        this.name = name;
    }

    public static void main(String[] args) {
        int result = 2;
        ClassDemo1();

        if (result == 2 && res == 65) {
            res = 0;
        }
        System.out.println(res);
    }


    public static void ClassDemo1() {
        Class class1 = ClassNativeUncover.class;
        test1(class1);
        test2(class1);
        test3(class1);
        test4(class1);
        test5(class1);
        test6(class1);
        test7(class1);
        test8(class1);
        test9(class1);
        test10(class1);
        test11(class1);
        test12(class1);
        test13(class1);
        test14(class1);
        test15(class1);
        test16(class1);
        test17(class1);
    }

    /**
     * public native Class<?> getDeclaringClass();
     *
     * @param class1
     */
    public static void test1(Class class1) {
        try {
            boolean flag = class1.isMemberClass();//getDeclaringClass() called by isMemberClass();
            if (!flag) {
                //System.out.println(flag);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
            ClassNativeUncover.res = ClassNativeUncover.res - 2;
        }
    }


    /**
     * public native Class<?> getEnclosingClass();
     *
     * @param class1
     */
    public static void test2(Class class1) {
        try {
            String string = class1.getCanonicalName();//getEnclosingClass() called by getCanonicalName();
            if (string.equals("ClassNativeUncover")) {
                //System.out.println(string);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * public native boolean isAnonymousClass();
     *
     * @param class1
     */
    public static void test3(Class class1) {
        try {
            boolean flag = class1.isAnonymousClass();
            if (!flag) {
                //System.out.println(flag);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * private native Field getPublicFieldRecursive(String name);
     *
     * @param class1
     */
    public static void test4(Class class1) {
        try {
            Field field = class1.getField("name");//get the whole attributes which named "public" and inherited from parent class;getPublicFieldRecursive() called by getField();
            if (field.toString().equals("public java.lang.String ClassNativeUncover.name")) {
                //System.out.println(field.toString());
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**
     * public native Field[] getDeclaredFields();
     *
     * @param class1
     */
    public static void test5(Class class1) {
        try {
            Field[] fields = class1.getDeclaredFields();//get all declarative attributes;
            if (fields != null && fields.length > 0) {
//                for(Field field:fields ) {
//                System.out.println(field);
//            }
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**
     * public native Method[] MethodsUnchecked(boolean publicOnly);
     *
     * @param class1
     */
    public static void test6(Class class1) {
        try {
            Method[] methods = class1.getDeclaredMethods();//get declarative methods;getDeclaredMethodsUnchecked() called by getDeclaredMethods();
            if (methods != null && methods.length > 0) {
//                for(Method method:methods ) {
//                System.out.println(method);
//            }
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**
     * private native Constructor<?>[] getDeclaredConstructorsInternal(boolean publicOnly);
     *
     * @param class1
     */
    public static void test7(Class class1) {
        try {
            Constructor[] constructors = class1.getConstructors();//get public constructions and inherited from parent class;getDeclaredConstructorsInternal()called bygetConstructors();
            if (constructors != null && constructors.length > 0) {
//                for(Constructor constructor:constructors ) {
//                System.out.println(constructor);
//            }
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * dcons
     * private native Constructor<?>[] getDeclaredConstructorsInternal(boolean publicOnly);
     *
     * @param class1
     */
    public static void test8(Class class1) {
        try {
            Constructor[] constructors = class1.getDeclaredConstructors();//get all of declarative construcors;getDeclaredConstructorsInternal() called by getDeclaredConstructors;
            if (constructors != null && constructors.length > 0) {
//            for(Constructor constructor:constructors ) {
//                System.out.println(constructor);
//            }
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**
     * public native Field getDeclaredField(String name) throws NoSuchFieldException;
     *
     * @param class1
     */
    public static void test9(Class class1) {
        try {
            Field field = class1.getDeclaredField("res");
            if (field.toString().equals("private static int ClassNativeUncover.res")) {
                //System.out.println(field);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }


    /**
     * private native Constructor<T> getDeclaredConstructorInternal(Class<?>[] args);
     *
     * @param class1
     */
    public static void test10(Class class1) {
        try {
            Constructor constructor = class1.getConstructor(new Class[]{String.class});//getDeclaredConstructorInternal() called by getConstructor;
            if (constructor.toString().equals("public ClassNativeUncover(java.lang.String)")) {
                //System.out.println(constructor);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    /**
     * private native Constructor<T> getDeclaredConstructorInternal(Class<?>[] args);
     *
     * @param class1
     */
    public static void test11(Class class1) {
        try {
            Constructor constructor = class1.getDeclaredConstructor(new Class[]{String.class});//getDeclaredConstructorInternal() called by getDeclaredConstructor();
            if (constructor.toString().equals("public ClassNativeUncover(java.lang.String)")) {
                //System.out.println(constructor);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }


    /**
     * private native String getInnerClassName();
     *
     * @param class1
     */
    public static void test12(Class class1) {
        try {
            String string = class1.getSimpleName();//getInnerClassName() called by getSimpleName();
            if (string.equals("ClassNativeUncover")) {
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
                //System.out.println(string);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * private native String[] getSignatureAnnotation();
     *
     * @param class1
     */
    public static void test13(Class class1) {
        try {
            TypeVariable[] typeVariables = class1.getTypeParameters();//return a array length 0;getSignatureAnnotation() called by getTypeParameters();
            if (typeVariables.getClass().toString().equals("class [Ljava.lang.reflect.TypeVariable;") && typeVariables.length == 0) {
                //System.out.println(typeVariables.length);
                //System.out.println(typeVariables.getClass().toString());
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * private native String[] getSignatureAnnotation();
     *
     * @param class1
     */
    public static void test14(Class class1) {
        try {
            Type type = class1.getGenericSuperclass();//getSignatureAnnotation()called by getGenericSuperclass;
            if (type.toString().equals("class java.lang.Object")) {
                //System.out.println(type);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * private native String[] getSignatureAnnotation();
     *
     * @param class1
     */
    public static void test15(Class class1) {

        Type[] type = class1.getGenericInterfaces();//getSignatureAnnotation() called by getGenericInterfaces;
        if (type.length == 0 && type.getClass().toString().equals("class [Ljava.lang.Class;")) {
            //System.out.println(type.length);
            //System.out.println(type.getClass().toString());
            ClassNativeUncover.res = ClassNativeUncover.res - 2;
        }
    }

    /**
     * private native Method getDeclaredMethodInternal(String name, Class<?>[] args);
     *
     * @param class1
     */
    public static void test16(Class class1) {
        try {
            Method method = class1.getMethod("ClassDemo1");//getDeclaredMethodInternal() called by getMethod()
            if (method.toString().equals("public static void ClassNativeUncover.ClassDemo1()")) {
                //System.out.println(method);
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    public static void test17(Class class1) {
        try {
            Annotation[] a = class1.getAnnotations(); //getDeclaredAnnotations() called by getAnnotations()
            if (a.length == 0 && a.getClass().toString().equals("class [Ljava.lang.annotation.Annotation;")) {
                //System.out.println(a.length);
                //System.out.println(a.getClass().toString());
                ClassNativeUncover.res = ClassNativeUncover.res - 2;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n