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
public class IteratorAndTemplateTest {
    public static void main(String args[]) {
        testIterator();
        testTemplate();
    }

    public static void testIterator(){
        AbstractCollection<String> l = new ArrayList<String>();
        l.add(new String("Hello"));
        l.add(new String(" World"));
        System.out.println(l.size());
        System.out.println(l.toString());
    }

    public static void testTemplate(){
        List<Integer> wholeChain = new ArrayList<Integer>();
        wholeChain.add(1);
        wholeChain.add(2);
        for (Integer i : wholeChain) {
            System.out.println(i);
        }
    }
}

