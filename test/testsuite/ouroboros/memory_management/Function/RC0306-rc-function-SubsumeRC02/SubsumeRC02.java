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
 * -@TestCaseID:maple/runtime/rc/function/SubsumeRC02.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: test subsumerc for RC, assign a value returned by function call to a field
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: SubsumeRC02.java
 *- @ExecuteClass: SubsumeRC02
 *- @ExecuteArgs:
 *- @ExpectMplCode:
 *- callassigned &Native_java_lang_StringFactory_newStringFromString__Ljava_lang_String_2 (iread ref <* <* <$Ljava_2Flang_2FString_3B>>> 0 (addrof ptr $_PTR_C_STR_7400650073007400)) { regassign ref %1}
 *- call &CC_WriteRefFieldNoInc (
 *-   regread ref %2,
 *-   iaddrof ref <* <$LSubsumeRC02_3B>> 4 (regread ref %2),
 *-   regread ref %1)
 */
public class SubsumeRC02 {
    private String str;

    public SubsumeRC02() {
    }

    public void testfunc() {
        str = new String("test");
        return;
    }

    public static void main(String[] args) {
        SubsumeRC02 temp = new SubsumeRC02();
        temp.testfunc();
        System.out.println("ExpectResult");
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n