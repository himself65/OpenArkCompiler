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
 * -@TestCaseID: Ouroboros/rt_test/SetMaxPriority
 *- @TestCaseName: Thread_SetMaxPriority.java
 *- @RequirementName: Java Thread
 *- @Title/Destination:test ThreadGroup.setMaxPriority ignores bad argument values
 *- @Brief: see below
 * -#step1: 以foo为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 通过调用threadGroup的getParent()方法，得其返回值并赋值给threadGroupParent；
 * -#step3: 将threadGroup.getMaxPriority()的返回值记为currentMaxPriority，将threadGroupParent.getMaxPriority() / 2的返回
 *          值记为halfMaxPriority；
 * -#step4: 经判断得知halfMaxPriority与Thread.MIN_PRIORITY的差值大于等于2；
 * -#step5: 以（halfMaxPriority - 2）为参数，调用threadGroup的setMaxPriority()方法，而后将threadGroup.getMaxPriority()的
 *          返回值赋值给currentMaxPriority；
 * -#step6: 经判断得知currentMaxPriority与（halfMaxPriority - 2）的值相等；
 * -#step7: 以（currentMaxPriority + 1）为参数，调用threadGroup的setMaxPriority()方法，而后将
 *          threadGroup.getMaxPriority()的返回值记为newMaxPriority；
 * -#step8: 经判断得知newMaxPriority与（currentMaxPriority + 1）的值相等；
 * -#step9: 以（Thread.MIN_PRIORITY - 1）为参数，调用threadGroup的setMaxPriority()方法；
 * -#step10: 经判断得知Thread.MIN_PRIORITY的值与threadGroup.getMaxPriority()的返回值不相等；
 * -#step11: 以（Thread.MAX_PRIORITY + 1）为参数，调用threadGroup的setMaxPriority()方法；
 * -#step12: 经判断得知Thread.MAX_PRIORITY的值与threadGroup.getMaxPriority()的返回值不相等；
 *- @Expect:expected.txt
 *- @Priority: High
 *- @Source: SetMaxPriority.java
 *- @ExecuteClass: SetMaxPriority
 *- @ExecuteArgs:
 */
/*
 * Copyright (c) 2007, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

/*
 * @test
 * @bug 4708197 6497629
 * @summary Test for max priority setting that matches spec
 * @author Pete Soper
 */

public class SetMaxPriority {

    public static void main(String[] args) throws Exception {
        ThreadGroup tg = new ThreadGroup("foo");
        ThreadGroup ptg = tg.getParent();
        int currentMaxPriority = tg.getMaxPriority();
        int halfMaxPriority = ptg.getMaxPriority() / 2;
        if (halfMaxPriority - Thread.MIN_PRIORITY < 2) {
            throw new RuntimeException("SetMaxPriority test no longer valid: starting parent max priority too close to Thread.MIN_PRIORITY");
        }
        tg.setMaxPriority(halfMaxPriority - 2);
        currentMaxPriority = tg.getMaxPriority();
        if (currentMaxPriority != halfMaxPriority - 2) {
            throw new RuntimeException("SetMaxPriority failed: max priority not changed");
        }

        // This will fail if bug 6497629 is present because the min tests is
        // being made with the (just lowered) max instead of the parent max,
        // preventing the priority from being moved back up.
        tg.setMaxPriority(currentMaxPriority + 1);
        int newMaxPriority = tg.getMaxPriority();
        if (newMaxPriority != currentMaxPriority + 1) {
            throw new RuntimeException("SetMaxPriority failed: defect 6497629 present");
        }

        // Confirm that max priorities out of range on both ends have no
        // effect.
        /* android is not same as openjdk*/
        tg.setMaxPriority(Thread.MIN_PRIORITY - 1);
        if (Thread.MIN_PRIORITY != tg.getMaxPriority())
            throw new RuntimeException(
                    "setMaxPriority bad arg not ignored as specified");

        tg.setMaxPriority(Thread.MAX_PRIORITY + 1);
        if (Thread.MAX_PRIORITY != tg.getMaxPriority())
            throw new RuntimeException(
                    "setMaxPriority bad arg not ignored as specified");

        System.out.println("SetMaxPriority passed");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan SetMaxPriority\s*passed