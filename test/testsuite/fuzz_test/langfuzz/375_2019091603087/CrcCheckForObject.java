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

import java.lang.annotation.Annotation;
import java.io.*;
import java.lang.reflect.*;
import java.util.*;
public class CrcCheckForObject {
    /**
     * 执行反射方法后返回可观测值，打印到标准输出流
     * <p>
     * param result1 执行反射方法后返回的Object类型 param method 执行反射需调用的方法 param instance
     * 方法调用的对象 param parameters 方法调用的参数列表 return null
     */
    private static boolean showDebug = false;
    public static int length = 0;
    private static List<Object> resultlist = new LinkedList<>();
    private static List<String> foundationtypes = new ArrayList<>();
    //apifuzz的whitelist.txt,現在不從文件里讀了    
    private static ArrayList<String> whitelist  = new ArrayList(Arrays.asList("static final boolean java.util.concurrent.atomic.AtomicLong.VM_SUPPORTS_LONG_CAS",
            "java.lang.reflect.AccessibleObject.override",
            "private final int java.lang.ThreadLocal.threadLocalHashCode",
            "private static java.util.concurrent.atomic.AtomicInteger java.lang.ThreadLocal.nextHashCode",
            "int java.lang.ThreadGroup.nthreads",
            "private static long java.lang.Thread.threadSeqNumber",
            "private long java.lang.Thread.tid",
            "private static final long java.util.concurrent.locks.ReentrantReadWriteLock.TID",
            "transient java.util.Set java.util.HashMap.entrySet",
            "private static java.lang.Throwable[] java.lang.Throwable.EMPTY_THROWABLE_ARRAY",
            "private final long libcore.util.NativeAllocationRegistry.freeFunction",
            "private transient long java.math.BigInt.bignum",
            "private static final int java.util.concurrent.ForkJoinPool.ABASE",
            "private static final long java.util.concurrent.ForkJoinPool.RUNSTATE",
            "private static final long java.io.File.PREFIX_LENGTH_OFFSET",
            "private static final long java.util.concurrent.locks.StampedLock.WSTATUS",
            "private static final long java.util.concurrent.locks.StampedLock.WTAIL",
            "private static final long java.util.concurrent.locks.StampedLock.STATE",
            "private static final long java.util.concurrent.locks.StampedLock.PARKBLOCKER",
            "private static final long java.util.concurrent.locks.StampedLock.WNEXT",
            "private final long java.util.SplittableRandom.gamma",
            "private volatile long java.util.concurrent.atomic.AtomicLong.value",
            "private static final java.util.concurrent.atomic.AtomicLong java.util.SplittableRandom.defaultGen",
            "private long java.util.SplittableRandom.seed",
            "private static final int java.util.concurrent.Exchanger.ABASE",
            "private static final long java.util.concurrent.Exchanger.BLOCKER",
            "private static final long java.util.concurrent.Exchanger.MATCH",
            "private static final long java.util.concurrent.Exchanger.BOUND",
            "private transient long java.util.Date.fastTime",
            "private transient sun.util.calendar.BaseCalendar$Date java.util.Date.cdate",
            "private final java.util.Date java.util.logging.SimpleFormatter.dat",
            "private transient int java.lang.Object.shadow$_monitor_",
            "private static final long java.util.concurrent.SynchronousQueue$TransferQueue$QNode.NEXT",
            "private static final long java.util.concurrent.SynchronousQueue$TransferQueue$QNode.ITEM",
            "private int java.io.BufferedWriter.nextChar",
            "private int java.lang.StackTraceElement.lineNumber",
            "private java.lang.String java.lang.StackTraceElement.methodName",
            "private static final long java.lang.StackTraceElement.serialVersionUID",
            "private java.lang.ClassLoader java.lang.Thread.contextClassLoader",
            "private static volatile java.lang.Thread$UncaughtExceptionHandler java.lang.Thread.uncaughtExceptionPreHandler ",
            "private static final long java.util.concurrent.ConcurrentHashMap.CELLSBUSY",
            "private static final long java.util.concurrent.ConcurrentHashMap.SIZECTL",
            "private static final long java.util.concurrent.ConcurrentHashMap.TRANSFERINDEX",
            "private static final long java.util.concurrent.ConcurrentHashMap.BASECOUNT",
            "private volatile long java.util.zip.ZStreamRef.address",
            "private static final long java.util.concurrent.PriorityBlockingQueue.ALLOCATIONSPINLOCK",
            "private static final long java.util.concurrent.LinkedTransferQueue.TAIL",
            "private static final long java.util.concurrent.LinkedTransferQueue.SWEEPVOTES",
            "private transient java.util.TreeMap$KeySet java.util.TreeMap.navigableKeySet",
            "private transient volatile java.lang.String java.util.Locale.languageTag",
            "private transient volatile int java.util.Locale.hashCodeValue",
            "private static volatile java.util.Locale java.util.Locale.defaultDisplayLocale",
            "private static final long java.util.concurrent.LinkedTransferQueue.TAIL",
            "private static final long java.util.concurrent.LinkedTransferQueue.SWEEPVOTES",
            "private static final long java.util.concurrent.ForkJoinPool.RUNSTATE",
            "private static final long java.util.concurrent.ForkJoinPool.CTL",
            "private static final long java.util.concurrent.SynchronousQueue$TransferQueue$QNode.NEXT",
            "private static final long java.util.concurrent.SynchronousQueue$TransferQueue$QNode.ITEM",
            "transient java.util.Set java.util.HashMap.entrySet",
            "private transient int java.lang.Object.shadow$_monitor_",
            "private transient java.util.TreeMap$KeySet java.util.TreeMap.navigableKeySet",
            "private transient volatile java.util.Set java.util.Hashtable.entrySet",
            "private static java.lang.Throwable[] java.lang.Throwable.EMPTY_THROWABLE_ARRAY",
            "private volatile long java.lang.Thread.nativePeer",
            "java.lang.ThreadLocal$ThreadLocalMap java.lang.Thread.threadLocals",
            "private static final int java.util.concurrent.ConcurrentHashMap.ABASE",
            "private static final int java.util.concurrent.ForkJoinPool.ABASE",
            "private static final long java.util.concurrent.ForkJoinPool.CTL",
            "private static final java.util.concurrent.atomic.AtomicLong java.util.Random.seedUniquifier",
            "private final java.util.concurrent.atomic.AtomicLong java.util.Random.seed, private static final long java.util.Random.seedOffset",
            "private static final long java.util.concurrent.Exchanger.SLOT",
            "private static final long java.util.concurrent.locks.StampedLock.WHEAD",
            "transient int java.util.HashMap.size",
            "transient int java.util.HashMap.modCount",
            "private static final long java.util.Random.seedOffset",
            "boolean java.lang.reflect.AccessibleObject.override",
            "private final java.util.concurrent.atomic.AtomicLong java.util.Random.seed",
            "private java.lang.String java.lang.StackTraceElement.fileName",
            "private java.lang.String java.lang.StackTraceElement.declaringClass",
            "private transient java.lang.Object java.lang.Class.vtable",
            "private transient java.lang.Class java.lang.Class.superClass",
            "private transient java.lang.String java.lang.Class.name",
            "private final java.lang.Class java.lang.reflect.Constructor.serializationClass",
            "private transient dalvik.system.ClassExt java.lang.Class.extData",
            "private transient long java.util.regex.Pattern.address",
            "private transient volatile java.lang.Object java.lang.Throwable.backtrace",
            "private transient java.lang.Object java.lang.Class.dexCache",
            "private transient java.lang.ClassLoader java.lang.Class.classLoader",
            "private transient java.lang.Class java.lang.Class.componentType",
            "private transient long java.lang.Class.sFields",
            "private transient int java.lang.Class.referenceInstanceOffsets",
            "private transient int java.lang.Class.primitiveType",
            "private transient int java.lang.Class.objectSizeAllocFastPath",
            "private transient int java.lang.Class.objectSize",
            "private transient int java.lang.Class.numReferenceStaticFields",
            "private transient int java.lang.Class.numReferenceInstanceFields",
            "private transient long java.lang.Class.methods",
            "private transient java.lang.Object[] java.lang.Class.ifTable",
            "private transient long java.lang.Class.iFields",
            "private transient volatile int java.lang.Class.dexTypeIndex",
            "private transient int java.lang.Class.dexClassDefIndex",
            "private transient short java.lang.Class.copiedMethodsOffset",
            "private transient int java.lang.Class.clinitThreadId",
            "private transient int java.lang.Class.classSize",
            "private transient int java.lang.Class.classFlags",
            "private transient int java.lang.Class.accessFlags",
            "private java.lang.Class java.lang.reflect.Field.declaringClass",
            "private final java.lang.Class java.lang.reflect.Constructor.serializationCtor",
            "private transient int java.lang.Class.status",
            "private transient short java.lang.Class.virtualMethodsOffset",
            "private int java.lang.reflect.Field.dexFieldIndex ",
            "private int java.lang.reflect.Field.offset",
            "private int java.lang.reflect.Field.accessFlags",
            "private int fuzzapiinvoke.getInstance.ObjectNewInstance$MyThread1.ticket",
            "private long java.util.regex.Matcher.address",
            "transient long java.util.regex.Pattern.address",
            "private int java.lang.reflect.Field.dexFieldIndex",
            "private char[] java.io.BufferedWriter.cb",
            "private java.lang.Class java.lang.reflect.Field.type"
    ));
    private static String[] ss = {"class java.lang.Character","class java.lang.Integer",
            "class java.lang.Short", "class java.lang.Long", "class java.lang.Float",
            "class java.lang.Double", "class java.lang.Char", "class java.lang.Boolean",
            "class java.lang.Byte", "class java.lang.String", "class java.lang.StringBuffer",
            "class java.lang.Character$UnicodeBlock", "class java.util.Locale",
            "class java.lang.StringBuilder", "class [I", "class [S", "class [J", "class [F", "class [D",
            "class [Z", "class [B", "class [C", "class [Ljava.lang.Character;",
            "class [Ljava.lang.String;", "class [Ljava.lang.StringBuffer;",
            "class [Ljava.lang.StringBuilder;"};  // 该数组存放此类可以处理用于计算CRC的数据类型
    private static ArrayList<String> dealtypes = new ArrayList<String>();

    //輸出每個Object單獨的CRC值
    public static List<Long> CRCListForObjects(Object... results) {
        List<Long> finalres = new ArrayList<>();
        for (Object result1: results) {
            try {
                if (showDebug) {
                    System.out.println("==============================  CRCcheck(Param info) ==============================");
                    System.out.println("| result: "+result1);
                    System.out.println("===================================================================================");
                }

                if (result1 != null) {  // resultthreadgroup isnot null, might be(object  object[])
                    if (result1.getClass().toString().startsWith("class [L") || result1.getClass().toString().startsWith("[L")) {  // resultthreadgroup is object[]
                        for (Object obj : (Object[]) result1) {
                            if (obj != null) {
                                finalres.add(run(obj)); // used to be "result1", solve the problem of object-array
                            } else {
                                finalres.add(1000000000L);
                            }
                        }
                    } else {
                        finalres.add(run(result1));  // resultthreadgroup is object}
                    }
                } else {
                    // result1 is null
                    finalres.add(1000000000L);
                }
            } catch (Exception e) {
                e.printStackTrace(System.out);
                System.exit(234);
                return null;
            }
        }
        return finalres;
    }

    //輸出Object數組總的CRC值
    public static long totalCRCForObjects(boolean showFieldInfo, Object... results){
        showDebug = showFieldInfo;
        initialize();
        for (Object result1: results) {
            try {
                if (result1 != null) {  // resultthreadgroup isnot null, might be(object  object[])
                    if (result1.getClass().toString().startsWith("class [L") || result1.getClass().toString().startsWith("[L")) {  // resultthreadgroup is object[]
                        for (Object obj : (Object[]) result1) {
                            if (obj != null) {
                                calculatecrc(obj);
                            } else {
//                                finalres.add(1000000000L);
                            }
                        }
                    } else {
                        calculatecrc(result1);
                    }
                } else {
                    // result1 is null
//                    finalres.add(1000000000L);
                }
            } catch (Exception e) {
                e.printStackTrace(System.out);
                System.exit(234);
            }
        }
        return getCRC();
    }

    public static long run(Object obj) throws Exception {
        initialize();
        //System.out.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  START GetDeclaredFields  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        calculatecrc(obj);
        //System.out.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    END GetDeclaredFields  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        return getCRC();
    }
    private static void initialize(){
        Collections.addAll(dealtypes, ss);
        String[] strings = {"class java.lang.Character","class java.lang.Integer", "class java.lang.Short", "class java.lang.Long", "class java.lang.Float", "class java.lang.Double", "class java.lang.Char", "class java.lang.Boolean", "class java.lang.Byte"};
        Collections.addAll(foundationtypes, strings);
        resultlist.clear();
    }

    private static void calculatecrc(Object result) throws Exception {
        // deal with Object
        if(result != null){
            String resulttype = result.getClass().toString();
            // deal with the problem of class StringBuilder, StringBuffer and String
            // we can`t get the specific fields we wanted, cast these to string by toString
            if(resulttype.equals("class java.lang.String") ||
                    resulttype.equals("class java.lang.StringBuilder") ||
                    resulttype.equals("class java.lang.StringBuffer") ||
                    resulttype.equals("class java.lang.Character$UnicodeBlock") ||
                    resulttype.equals("class java.util.Locale") || result instanceof Annotation){
                String ostring = result.toString();
                byte[] bytes = ostring.getBytes("UTF-8");
                calculatecrc(bytes);
                return;
            }
            if (foundationtypes.contains(result.getClass().toString())){
                // if  basic types:
                resultlist.add(result);
                switch (result.getClass().toString()) {
                    case "class java.lang.Byte":
                        length += 1;
                        break;
                    case "class java.lang.Short":
                        length += 2;
                        break;
                    case "class java.lang.Integer":
                        length += 4;
                        break;
                    case "class java.lang.Long":
                        length += 8;
                        break;
                    case "class java.lang.Float":
                        length += 4;
                        break;
                    case "class java.lang.Double":
                        length += 8;
                        break;
                    case "class java.lang.Char":
                        length += 2;
                        break;
                    case "class java.lang.Character":
                        length += 2;
                        break;
                    case "class java.lang.Boolean":
                        length += 1;
                        break;}
            } else if(resulttype.startsWith("class [")) {
                //  array(contains multidimensional array)
                if ("class [I".equals(resulttype)) {
                    //int[]
                    int[] intparam = (int[]) result;
                    for (int i : intparam) {
                        resultlist.add(i);
                        length += 4;
                    }
                } else if ("class [S".equals(resulttype)) {
                    //short[]
                    short[] shortparam = (short[]) result;
                    for (short s : shortparam) {
                        resultlist.add(s);
                        length += 2;
                    }
                } else if ("class [J".equals(resulttype)) {
                    //long[]
                    long[] longparam = (long[]) result;
                    for (long l : longparam) {
                        resultlist.add(l);
                        length += 8;
                    }
                } else if ("class [F".equals(resulttype)) {
                    //float[]
                    float[] floatparam = (float[]) result;
                    for (float f : floatparam) {
                        resultlist.add(f);
                        length += 4;
                    }
                } else if ("class [D".equals(resulttype)) {
                    //double[]
                    double[] doubleparam = (double[]) result;
                    for (double d : doubleparam) {
                        resultlist.add(d);
                        length += 8;
                    }
                } else if ("class [Z".equals(resulttype)) {
                    //boolean[]
                    boolean[] booleanparam = (boolean[]) result;
                    for (boolean b : booleanparam) {
                        resultlist.add(b);
                        length += 1;
                    }
                } else if ("class [B".equals(resulttype)) {
                    //byte[]
                    byte[] byteparam = (byte[]) result;
                    for (byte b : byteparam) {
                        resultlist.add(b);
                        length += 1;
                    }
                } else if ("class [C".equals(resulttype)) {
                    //char[]
                    char[] charparam = (char[]) result;
                    for (char c : charparam) {
                        resultlist.add(c);
                        length += 2;
                    }
                } else if ("class [Ljava.lang.Character;".equals(resulttype)) {
                    // Character[]
                    Character[] charparam = (Character[]) result;
                    for (Character c : charparam) {
                        resultlist.add((char)c);
                        length += 2;
                    }
                }else if (resulttype.startsWith("class [[")){
                    // 多维数组，降维
                    for (Object o: (Object[]) result){
                        calculatecrc(o);
                    }
                }else if (resulttype.equals("[Ljava.lang.String")){
                    // string类型的数组
                    for(String s:(String[]) result){
                        calculatecrc(s);
                    }

                }
                else if(resulttype.startsWith("class [L")){
                    // 非基础类型数组
                    for(Object o:(Object[])result){
                        calculatecrc(o);
                    }
                }
            }else{
                // 该分支处理非基础类型对象, 先获取对象下指定层数的域和值,将最终结果中可用于计算CRC校验的值作为参数递归调用本方法
                // 首先取得首次传入的实例的域,若域的值可直接用于(包括基础类型和在本方法中特殊处理的其他类型)CRC计算,则不进入下一次获取域的循环
                // 最终获取的域和对应的值都有序的存放在getfaieldvalue()返回的map中
                // 接着对map进行遍历获取其中的值如若是本方法除此分支之外的其他分支可处理的类型则以value为参数递归调用,否则只将其输出在运行日志中
                //System.out.println("```````````````````````````get field`````````````````````````````````````````````");
                Map<Field, Object>map = getFieldValue(result, 3);
                //System.out.println("`````````````````````````get field end```````````````````````````````````````````");
                for(Map.Entry<Field, Object> entry: map.entrySet()) {
                    Field key = entry.getKey();
                    Object value = entry.getValue();
                    //当field为基础类型或者string stringbu*** character数组时将数组信息打印出来
                    String[] basicarray = new String[]{"class [I", "class [S", "class [J", "class [F", "class [D", "class [Z", "class [B", "class [C", "class [Ljava.lang.Character;", "class [Ljava.lang.String;", "class [Ljava.lang.StringBuffer;", "class [Ljava.lang.StringBuilder;"};
                    List<String> basiclist = Arrays.asList(basicarray);
                    if(showDebug) {
                        if (basiclist.contains(key.getType().toString())) {
                            String str = key.getType().toString();
                            switch (str) {
                                case "class [I":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((int[]) value));
                                    break;
                                case "class [S":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((short[]) value));
                                    break;
                                case "class [J":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((long[]) value));
                                    break;
                                case "class [F":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((float[]) value));
                                    break;
                                case "class [D":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((double[]) value));
                                    break;
                                case "class [Z":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((boolean[]) value));
                                    break;
                                case "class [B":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((byte[]) value));
                                    break;
                                case "class [C":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((char[]) value));
                                    break;
                                case "class [Ljava.lang.Character;":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((Character[]) value));
                                    break;
                                case "class [Ljava.lang.String;":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((String[]) value));
                                    break;
                                case "class [Ljava.lang.StringBuffer;":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((StringBuffer[]) value));
                                    break;
                                case "class [Ljava.lang.StringBuilder;":
                                    System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.toString((StringBuilder[]) value));
                                    break;
                            }
                        } else if (key.getType().toString().contains("[[")) {
                            System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + Arrays.deepToString((Object[]) value));
                        } else {
                            System.out.println("< Which Field: >" + key + ";  < Field's Value: >" + value);
                        }
                    }

                    if(value != null && dealtypes.contains(value.getClass().toString())){
                        if(!whitelist.contains(key.toString())){
                            // 如果为基础类型或者需要特殊处理的object类型且没在whitelist中则再次调用本身,否则只在控制台打印出来对象信息
                            calculatecrc(value);
                        }
                    }
                }
            }
        }
    }
    /*
        接受object类型参数,向下获取实例类的域两次
        : reutrn 返回包含域对象和对应的value的键值对的map
     */
    private static Map<Field, java.lang.Object > getFieldValue(java.lang.Object obj, int length){
        Map<Field, java.lang.Object> map = new LinkedHashMap<>();
        Field[] rootf = obj.getClass().getDeclaredFields();
        //sort
        rootf = CrcCheckForObject.sortFieldArray(rootf);
        if(rootf == null){
            return map;
        }
        List<Object> cur_objs = new ArrayList<>();
        List<Object> next_objs = new ArrayList<>();
        // 先获取传入参数的域和对应的值, 放入map中, value 放入 cur_objs中
        //System.out.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^root field : value^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        for(Field f: rootf){
            if(whitelist.contains(f.toString())){
                continue;
            }
            //System.out.println("< which field: >" + f.toString());
            f.setAccessible(true);
            java.lang.Object o;
            try {
                o = f.get(obj);
                map.put(f,o);
            } catch (IllegalAccessException e) {
                o = null;
                map.put(f, null);
            }
            next_objs.add(o);
            //System.out.println("< field value: >" + o);
        }
        //System.out.println("------------------------------root end------------------------------------");
        while( length > 1){
            cur_objs.clear();
            cur_objs.addAll(next_objs);
            next_objs.clear();
            for(Object o:cur_objs){
                if(o == null ){
                    continue;
                }
                if(dealtypes.contains(o.getClass().toString())){
                    //如果为基础类型则不向下获取实例的域
                    continue;
                }
                Field[] fs = o.getClass().getDeclaredFields();
                fs = sortFieldArray(fs);
                if(fs == null){
                    continue;
                }
                for(Field fi: fs){

                    //System.out.println("< which field: >" + fi.toString());
                    if(whitelist.contains(fi.toString())){
                        continue;
                    }
                    fi.setAccessible(true);
                    Object oc;
                    try{
                        oc = fi.get(o);
                    }catch(IllegalAccessException e){
                        oc = null;
                    }
                    //System.out.println("< field value: >" + oc);

                    map.put(fi, oc);
                    next_objs.add(oc);
                }
            }
            length -= 1;
        }

        return map;
    }

    private static long getCRC () {
        CrcBuffer b = new CrcBuffer(length);
        for (Object obj:resultlist){
            if (obj.getClass().toString().equals("class java.lang.Byte")) {
                CrcCheckwithChar.ToByteArray((byte) obj, b);
            } else if (obj.getClass().toString().equals("class java.lang.Short")) {
                CrcCheckwithChar.ToByteArray((short) obj, b);
            } else if (obj.getClass().toString().equals("class java.lang.Integer")) {
                CrcCheckwithChar.ToByteArray((int) obj, b);
            } else if (obj.getClass().toString().equals("class java.lang.Long")) {
                CrcCheckwithChar.ToByteArray((long) obj, b);
            } else if (obj.getClass().toString().equals("class java.lang.Float")) {
                CrcCheckwithChar.ToByteArray((float) obj, b);
            } else if (obj.getClass().toString().equals("class java.lang.Double")) {
                CrcCheckwithChar.ToByteArray((double) obj, b);
            } else if (obj.getClass().toString().equals("class java.lang.Boolean")) {
                CrcCheckwithChar.ToByteArray((boolean) obj, b);
            } else if (obj.getClass().toString().equals("class java.lang.Character")) {
                CrcCheckwithChar.ToByteArray((char) obj, b);
            }
        }
        CRC32 c = new CRC32();
        c.update(b.buffer, 0, b.i);
        return c.getValue();
    }

    /**
     * 进行Field[]排序
     */
    public static Field[] sortFieldArray (Field[] fields){
        if (fields == null || fields.length == 0) {
            return null;
        }
        List<Field> flist = Arrays.asList(fields);
        Collections.sort(flist, new FieldComparator());

        return (Field[]) flist.toArray();
    }
}
/*
 * sort the field array by field`s name
 *
 * */
class FieldComparator implements Comparator<Field>{
    @Override
    public int compare(Field f1, Field f2){
        return (f1.toString()).compareTo(f2.toString());
    }

}


class CRC32 {
    byte[] buffer;
    int crc ;
    public void CRC32() {
        this.buffer = null;
    }


    public void update(byte[] b, int off, int len) {

        this.buffer = new byte[len];
        for (int i = 0; i < len; i++) {
            this.buffer[i] = b[i + off];
        }

    }

    private int getCrcByLookupTable() {

        int[] table = {
                0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
                0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
                0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
                0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
                0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
                0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
                0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
                0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
                0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
                0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
                0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
                0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
                0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
                0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
                0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
                0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
                0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
                0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
                0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
                0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
                0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
                0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
                0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
                0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
                0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
                0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
                0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
                0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
                0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
                0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
                0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
                0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
        };

        int crc = 0xFFFFFFFF;
        for (byte b : this.buffer) {
            crc = table[(crc ^ b) & 0xFF] ^ (crc >>> 8);
        }
        return crc ^ 0xFFFFFFFF;
    }


    public int getCrcByCalculation() {

        int poly = 0xEDB88320;
        int crc = 0xFFFFFFFF;
        
        for (byte b : this.buffer) {
            int value = (crc ^ b) & 0xFF;
            for (int i = 0; i < 8; i++) {
                if (1 == (value & 1)) {
                    value = (value >>> 1) ^ poly;
                } else {
                    value = value >>> 1;
                }
            }
            crc = (crc >>> 8) ^ value;
        }
        return crc ^ 0xFFFFFFFF;
    }

    public long getValue() {
        int crcTable = getCrcByLookupTable();
        /*
        int crcCal = getCrcByCalculation();
        if( crcCal != crcTable) {
            System.err.printf("
Fail to Get CRC!
	Calculation=%d	LookupTable=%d
",crcCal,crcValue);
            System.exit(2);
        }
        */
        return (long) crcTable & 0xffffffffL;
    }

}
