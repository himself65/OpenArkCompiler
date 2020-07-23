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
 * -@TestCaseID: ReflectionGetEnumConstants
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetEnumConstants.java
 *- @Title/Destination: Returns an array of the values of the enumeration classes that are represented in the order
 *                      declared by this class object.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法分别获取Weekday类、GetEnumConstants类的运行时类clazz1、clazz2；
 * -#step2: 分别通过clazz1.getEnumConstants()和clazz2.getEnumConstants()获取枚举类的数组并记为objects1、objects2；
 * -#step3: 确定step2中成功获取到枚举类数组objects1、objects2，并且objects1数组的长度为7，objects2数组为null；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetEnumConstants.java
 *- @ExecuteClass: ReflectionGetEnumConstants
 *- @ExecuteArgs:
 */

enum Weekday {
    MONDAY {
        Object test1() {
            class classA {
            }

            return new classA();
        }
    },
    TUESDAY {
        Object test2() {
            class classB {
            }

            return new classB();
        }
    },
    WEDNESDAY {
        Object test3() {
            class classC {
            }

            return new classC();
        }
    },
    THURSDAY {
        Object test4() {
            class classD {
            }

            return new classD();
        }
    },
    FRIDAY {
        Object test5() {
            class classE {
            }

            return new classE();
        }
    },
    SATURDAY {
        Object test6() {
            class classF {
            }

            return new classF();
        }
    },
    SUNDAY {
        Object test7() {
            class classG {
            }

            return new classG();
        }
    }
}

class GetEnumConstants {
}

public class ReflectionGetEnumConstants {
    public static void main(String[] args) {
        try {
            Class clazz1 = Class.forName("Weekday");
            Class clazz2 = Class.forName("GetEnumConstants");
            Object[] objects1 = clazz1.getEnumConstants();
            Object[] objects2 = clazz2.getEnumConstants();
            if (objects1.length == 7) {
                if (objects2 == null) {
                    System.out.println(0);
                }
            }
        } catch (ClassNotFoundException e) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n