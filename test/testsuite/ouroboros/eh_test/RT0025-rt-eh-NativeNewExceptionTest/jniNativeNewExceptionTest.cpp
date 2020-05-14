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
#include <stdio.h>
#include <jni.h>

extern "C" {
JNIEXPORT void JNICALL Java_NativeNewExceptionTest_nativeNativeNewExceptionTest__
(JNIEnv *env, jobject j_obj)
{
    jclass cls = NULL;
    jmethodID mid = NULL;

    cls = env->GetObjectClass(j_obj);
    if (cls == NULL){
        return;
    }
    mid = env->GetMethodID(cls,"callback", "()V");
    if (mid == NULL){
        return;
    }

    env->CallVoidMethod(j_obj, mid);
    jclass newExcCls = NULL;
    newExcCls = env->FindClass("java/lang/StringIndexOutOfBoundsException");
    if (newExcCls == NULL){
        return;
    }

    env->ThrowNew(newExcCls,"NativeThrowNew");
    printf("------>CheckPoint:CcanContinue\n");
}
}
