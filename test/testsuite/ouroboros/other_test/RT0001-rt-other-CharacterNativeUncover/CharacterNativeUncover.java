/**
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
 * -@TestCaseID: natives/CharacterNativeUncover.java
 * -@Title/Destination: Character Methods
 * -@Brief:
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: CharacterNativeUncover.java
 * -@ExecuteClass: CharacterNativeUncover
 * -@ExecuteArgs:
 */

public class CharacterNativeUncover {
    private static int res = 99;

    public static void main(String[] args) {
        int result = 2;
        CharacterDemo1();
        if (result == 2 && res == 53) {
            res = 0;
        }
        System.out.println(res);
    }


    public static void CharacterDemo1() {
        Character character = new Character('a');
        test1(character);
        test2(character);
        test3(character);
        test4(character);
        test5(character);
        test6(character);
        test7(character);
        test8(character);
        test9(character);
        test10(character);
        test11(character);
        test12(character);
        test13(character);
        test14(character);
        test15(character);
        test16(character);
        test17(character);
        test18(character);
        test19(character);
        test20(character);
        test21(character);
        test22(character);
        test23(character);
    }


    /**
     * static native boolean isLowerCaseImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test1(Character character) {
        try {
            boolean flag = Character.isLowerCase(97);//isLowerCaseImpl() called by isLowerCase();
            // System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isUpperCaseImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test2(Character character) {
        try {
            boolean flag = Character.isUpperCase(65);//isUpperCaseImpl() called by isUpperCase();
            //System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isTitleCaseImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test3(Character character) {
        try {
            boolean flag = Character.isTitleCase(0x01c8);//isTitleCaseImpl() called by isTitleCase();
            //System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isDigitImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test4(Character character) {
        try {
            boolean flag = Character.isDigit(0x06f8);//isDigitImpl() called by isDigit();judge if it`s number;
            //System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isDefinedImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test5(Character character) {
        try {
            boolean flag = Character.isDefined(0x012345);//isDefinedImpl()  called by isDefined();
            //System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }

    /**
     * static native boolean isLetterImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test6(Character character) {
        try {
            boolean flag = Character.isLetter(0x0065);//isLetterImpl() called by isLetter();tell if it`s char type;
            //System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isLetterOrDigitImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test7(Character character) {
        try {
            boolean flag = Character.isLetterOrDigit(0x0033);//isLetterOrDigitImpl() called by isLetterOrDigit();tell if it`s char type;
            //System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isAlphabeticImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test8(Character character) {
        try {
            boolean flag = Character.isAlphabetic(0x0065);//isAlphabeticImpl() called by  isAlphabetic(); tell if it`s char type;
            //System.out.println(flag);
            if (flag) {
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isIdeographicImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test9(Character character) {
        try {
            boolean flag = Character.isIdeographic(0x0065);//isIdeographicImpl() called by isIdeographic();

            if (!flag) {
                //System.out.println(flag);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isUnicodeIdentifierStartImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test10(Character character) {
        try {
            boolean flag = Character.isUnicodeIdentifierStart(0x0065);//isUnicodeIdentifierStartImpl() called by isUnicodeIdentifierStart();

            if (flag) {
                //System.out.println(flag);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isUnicodeIdentifierPartImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test11(Character character) {
        try {
            boolean flag = Character.isUnicodeIdentifierPart(0x053e);//isUnicodeIdentifierPartImpl() called by isUnicodeIdentifierPart();

            if (flag) {
                //System.out.println(flag);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * ensure the char should be a ignoreable char in java  or unicode identifier
     * static native boolean isIdentifierIgnorableImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test12(Character character) {
        try {
            boolean flag = Character.isIdentifierIgnorable(0x053e);//isIdentifierIgnorableImpl() called by isIdentifierIgnorable();

            if (!flag) {
                //System.out.println(flag);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native int toLowerCaseImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test13(Character character) {
        try {
            int num = Character.toLowerCase(0x053e);//toLowerCaseImpl() called by toLowerCase();

            if (num == 1390) {
                //System.out.println(num);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }

    /**
     * static native int toUpperCaseImpl(int codePoint)
     *
     * @param character
     * @return
     */
    public static boolean test14(Character character) {
        try {
            int num = Character.toUpperCase(0x053e);//toUpperCaseImpl() called by toUpperCase();

            if (num == 1342) {
                //System.out.println(num);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native int toTitleCaseImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test15(Character character) {
        try {
            int num = Character.toTitleCase(0x053e);//toTitleCaseImpl() called by toTitleCase();

            if (num == 1342) {
                //System.out.println(num);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }

    /**
     * native static int digitImpl(int codePoint, int radix);
     *
     * @param character
     * @return
     */
    public static boolean test16(Character character) {
        try {
            int num = Character.digit(9, 2);//digitImpl() called by digit();

            if (num == -1) {
                //System.out.println(num);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * native static int getNumericValueImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test17(Character character) {
        try {
            int num = Character.getNumericValue(9);//getNumericValueImpl() called by getNumericValue();

            if (num == -1) {
                //System.out.println(num);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native boolean isSpaceCharImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test18(Character character) {
        try {
            boolean flag = Character.isSpaceChar(9);//isSpaceCharImpl()  called by isSpaceChar();

            if (!flag) {
                //System.out.println(flag);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * native static boolean isWhitespaceImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test19(Character character) {
        try {
            boolean flag = Character.isWhitespace(9);//isWhitespaceImpl() called by isWhitespace();

            if (flag) {
                //System.out.println(flag);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * static native int getTypeImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test20(Character character) {
        try {
            int num = Character.getType(9);//getTypeImpl() called by getType();

            if (num == 15) {
                //System.out.println(num);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }

    /**
     * native static byte getDirectionalityImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test21(Character character) {
        try {
            int num = Character.getDirectionality(9);//getDirectionalityImpl() called by getDirectionality();

            if (num == 11) {
                //System.out.println(num);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * native static boolean isMirroredImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test22(Character character) {
        try {
            boolean flag = Character.isMirrored(9);//isMirroredImpl() called by isMirrored();

            if (!flag) {
                //System.out.println(flag);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }


    /**
     * private static native String getNameImpl(int codePoint);
     *
     * @param character
     * @return
     */
    public static boolean test23(Character character) {
        try {
            String string = Character.getName(9);//getNameImpl() called by getName();

            //if (string.equals("CHARACTER TABULATION")) {
            if (string.equals("<control-0009>")) {
                //System.out.println(string);
                CharacterNativeUncover.res = CharacterNativeUncover.res - 2;
            } else {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n