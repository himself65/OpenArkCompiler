#!/usr/bin/env python
# coding=utf-8
#
# Copyright (c) [2020] Huawei Technologies Co.,Ltd.All rights reserved.
#
# OpenArkCompiler is licensed under the Mulan PSL v1.
# You can use this software according to the terms and conditions of the Mulan PSL v1.
# You may obtain a copy of Mulan PSL v1 at:
#
#     http://license.coscl.org.cn/MulanPSL
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
# FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v1 for more details.
#
import os, sys, subprocess, shlex, re
def Gendef(execTool, mdFiles, outputDir):
  tdList = []
  for mdFile in mdFiles:
    if mdFile.find('sched') >= 0:
      schedInfo = mdFile
      mdCmd = "%s --genSchdInfo %s -o %s" %(execTool, schedInfo , outputDir)
      isMatch = re.search(r'[;\\|\\&\\$\\>\\<`]', mdCmd, re.M|re.I)
      if (isMatch):
        print("Command Injection !")
        return
      print("[*] %s" % (mdCmd))
      subprocess.check_call(shlex.split(mdCmd), shell = False)
    else:
      tdList.append(i)
  return

def Process(execTool, mdFileDir, outputDir):
  if not (os.path.exists(execTool)):
    print("maplegen is required before generating def files automatically")
    return
  if not (os.path.exists(mdFileDir)):
    print("td/md files is required as input!!!")
    print("Generate def files FAILED!!!")
    return

  mdFiles = []
  for root,dirs,allfiles in os.walk(mdFileDir):
    for mdFile in allfiles:
      mdFiles.append("%s/%s"%(mdFileDir, mdFile))

  if not (os.path.exists(outputDir)):
    print("Create the " + outputDir)
    os.makedirs(outputDir)
    Gendef(execTool, mdFiles, outputDir)

  defFile = "%s/mplad_arch_define.def" % (outputDir)
  if not (os.path.exists(defFile)):
    Gendef(execTool, mdFiles, outputDir)
  for mdfile in mdFiles:
    if (os.stat(mdfile).st_mtime > os.stat(defFile).st_mtime):
      Gendef(execTool, mdFiles, outputDir)
  if (os.stat(execTool).st_mtime > os.stat(defFile).st_mtime):
    Gendef(execTool, mdFiles, outputDir)

def help():
  print("Usage: %s maplegen_exe_directory mdfiles_directory output_defiless_directory" % (sys.argv[0]));

def main():
  if len(sys.argv) != 4:
    help();
    return
  Process(sys.argv[1], sys.argv[2], sys.argv[3])
if __name__ == "__main__":
  main()
