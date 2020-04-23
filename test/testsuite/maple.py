#!/usr/bin/python3
# -*- coding:utf-8 -*-
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

import argparse
import collections
import logging
import os
import subprocess
import time
import re
from functools import wraps


limitation = 0
output_name = ""
input_name = ""
logger = logging.getLogger("COMPILE")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
# create formatter and add it to the handlers
formatter = logging.Formatter("%(asctime)-18s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
# add the handlers to the logger
logger.addHandler(ch)


def timing(func):
    # Decorator that reports the execution time.
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        logger.info("Time Consuming:" + str(end - start) + "s")
        return result

    return wrapper


# exec the compile command
@timing
def build(cmd):
    global limitation
    run_cmd = " ".join(cmd)
    logger.info(run_cmd)
    com = subprocess.Popen(run_cmd,
                           stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE,
                           shell=True,
                           cwd="./",
                           universal_newlines=True)
    com_out, com_err = "", ""
    try:
        com_out, com_err = com.communicate(timeout=limitation)
        if com_out is not None:
            logger.debug(com_out)
        if not com.returncode == 0:
            logger.error(com_err)
            exit(1)
    except subprocess.TimeoutExpired as e:
        com.kill()
        logger.error(e)
        exit(3)
    except Exception as ee:
        com.kill()
        logger.error(ee)
        exit(1)
    return com_out, com_err, com.returncode


class MultiCompiler:
    def __init__(self, name, in_suffix, out_suffix, options):
        self._in_files_name = ""
        self._out_file_name = ""
        self._is_skipped = True
        self._name = name
        self._in_suffix = in_suffix
        self._out_suffix = out_suffix
        self._options = options

    def get_in_suffix(self):
        return self._in_suffix

    def update_info(self, path, options):
        if path is not None:
            self._name = path + self._name
        if options is not None:
            self._options = self._options + " " + options

    def prepare(self, file_list):
        temp_list = file_list.copy()
        for in_file in temp_list:
            name = in_file[0:-len(self._in_suffix)]
            if in_file == name + self._in_suffix:
                self._is_skipped = False
                file_list.remove(in_file)
                self._in_files_name = self._in_files_name + in_file + " "
                self._out_file_name = name + self._out_suffix
                file_list.append(self._out_file_name)
            else:
                continue

    def build(self):
        if not self._is_skipped:
            return build(self._gen_cmd())

    def _gen_cmd(self):
        cmd = [self._name]
        cmd.extend(self._make_opts())
        cmd.extend(re.sub(r"\s+", " ", self._in_files_name).strip().split(" "))
        return cmd

    def _make_opts(self):
        options = self._options.strip()
        opts = []
        opts.append(options)
        return opts


class Javac(MultiCompiler):
    def prepare(self, file_list):
        temp_list = file_list.copy()
        for in_file in temp_list:
            name = in_file[0:-len(self._in_suffix)]
            if in_file == name + self._in_suffix:
                self._is_skipped = False
                file_list.remove(in_file)
                self._in_files_name = self._in_files_name + in_file + " "
            else:
                continue
        if not self._is_skipped:
            self._out_file_name = "*.class"
            file_list.append(self._out_file_name)


class Jar(MultiCompiler):
    def prepare(self, file_list):
        global output_name
        temp_list = file_list.copy()
        for in_file in temp_list:
            name = in_file[0:-len(self._in_suffix)]
            if in_file == name + self._in_suffix:
                self._is_skipped = False
                file_list.remove(in_file)
                self._in_files_name = self._in_files_name + in_file + " "
            else:
                continue
        # Use the first input file name as the output file name
        if not self._is_skipped:
            self._out_file_name = output_name.split("/")[-1].split(".")[0] + self._out_suffix
            file_list.append(self._out_file_name)

    def _gen_cmd(self):
        # Specify the output file name
        cmd = [self._name]
        cmd.extend(self._make_opts())
        cmd.extend(re.sub(r"\s+", " ", self._out_file_name).strip().split(" "))
        cmd.extend(re.sub(r"\s+", " ", self._in_files_name).strip().split(" "))
        return cmd


class SingleCompiler(MultiCompiler):
    def prepare(self, file_list):
        is_single = 0
        temp_list = file_list.copy()
        for in_file in temp_list:
            name = in_file[0:-len(self._in_suffix)]
            if in_file == name + self._in_suffix:
                self._is_skipped = False
                is_single = is_single + 1
                if is_single > 1:
                    logger.error("Not support multi .jar")
                    exit(1)
                file_list.remove(in_file)
                self._in_files_name = in_file
                self._out_file_name = name + self._out_suffix
                file_list.append(self._out_file_name)
            else:
                continue


class Ld(MultiCompiler):
    def _gen_cmd(self):
        # Specify the output file name
        cmd = [self._name]
        cmd.extend(re.sub(r"\s+", " ", self._in_files_name).strip().split(" "))
        cmd.extend(self._make_opts())
        return cmd


# ------------------------------------------------------------------------
#
# initial parser and parse the args
# Add parser arguments by tool chain components name
#
# ------------------------------------------------------------------------
def do_init(components):
    parser = argparse.ArgumentParser(prog="maple", usage="maple [options] <inputs>")
    # choose toolchain target
    parser.add_argument("-p", "--platform", dest="target", choices=["aarch64"], default="aarch64",
                        help="choose toolchain target, default is aarch64")
    # Select compile phase
    parser.add_argument("-s", "--stage", dest='stage', choices=["javac", "jar", "maple", "as", "ld"], default="ld",
                        help="Select the compiler stage, default is ld")
    # set timeout limitation
    parser.add_argument("--timeout", metavar="TIMEOUT", dest="timeout", default=None, type=int,
                        help="set the timeout limitation")
    # set the output name
    parser.add_argument("-o", metavar="OUTPUT", dest="output", default="",
                        help="set the output file name")
    suffixes = set()
    for name in components.keys():
        parser.add_argument("--" + name, metavar=name.upper(),
                            help="set the options for component " + name)
        suffixes.add(components[name].get_in_suffix())
    # check the legal inputs
    parser.add_argument("filename", metavar="<inputs>", nargs="+",
                        help="legal input files include: " + str(suffixes))
    args = parser.parse_args()
    return vars(args)


# ------------------------------------------------------------------------
#
#  set the tool path and compile options
#
# -----------------------------------------------------------------------
def do_prepare(components, info, maple_root):
    global output_name
    maple_out_path = maple_root + "/output"
    maple_out_lib_path = maple_out_path + "/ops"
    maple_out_bin_path = maple_out_path + "/bin/"
    gnu_bin_path = maple_root + "/tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/"
    javac_options = "-g -d . "
    jar_options = "-cvf "
    as_options = "-g3 -O2 -x assembler-with-cpp -march=armv8-a -target aarch64-linux-gnu -c "
    linker_options = "-g3 -O2 -march=armv8-a -target aarch64-linux-gnu -fPIC -shared -o "\
                   + output_name + " " + maple_out_lib_path + "/mrt_module_init.o "\
                   + "-fuse-ld=lld -rdynamic -lcore-all -lcommon-bridge -Wl,-z,notext -Wl,-T"\
                   + maple_out_lib_path + "/linker/maplelld.so.lds"

    components["javac"].update_info(None, info["javac"])
    components["jar"].update_info(None, info["jar"])
    components["maple"].update_info(maple_out_bin_path, info["maple"])
    components["as"].update_info(gnu_bin_path, info["as"])
    components["ld"].update_info(gnu_bin_path, info["ld"])

    components["javac"].update_info(None, javac_options)
    components["jar"].update_info(None, jar_options)
    components["as"].update_info(None, as_options)
    components["ld"].update_info(None, linker_options)


# ------------------------------------------------------------------------
#
#  Select compile phase: "javac", "jar", "maple", "as", "ld"
#
# -----------------------------------------------------------------------
def do_update(components, file_type):
    if file_type == "javac":
        components.pop('jar')
        components.pop('maple')
        components.pop('as')
        components.pop('ld')
    elif file_type == "jar":
        components.pop('maple')
        components.pop('as')
        components.pop('ld')
    elif file_type == "maple":
        components.pop('as')
        components.pop('ld')
    elif file_type == "as":
        components.pop('ld')


# ------------------------------------------------------------------------
#
#  compile files one by one
#
# -----------------------------------------------------------------------
def do_check(components, file_list):
    # del the repeat input files
    for in_file in file_list:
        while file_list.count(in_file) > 1:
            file_list.remove(in_file)
        if not os.path.isfile(in_file):
            logger.error("No such file or directory: " + in_file)
            exit(1)
    # compile accord with pre-install phases
    for phase in list(components.keys()):
        components[phase].prepare(file_list)


# ------------------------------------------------------------------------
#
#  Main entry for maple java driver
#
# ------------------------------------------------------------------------
def main():
    maple_root = os.environ.get("MAPLE_ROOT")
    if maple_root is None:
        logger.error("Not found system environment: MAPLE_ROOT")
        exit(1)
    # initial all tool chain components:
    # set the default name, input suffix, output suffix, and if it support multi-inputs
    components = collections.OrderedDict()
    components["javac"] = Javac("/usr/bin/javac", ".java", ".class", "")
    components["jar"] = Jar("/usr/bin/jar", ".class", ".jar", "")
    components["maple"] = SingleCompiler("maple", ".jar", ".VtableImpl.s", "")
    components["as"] = MultiCompiler("clang++", ".s", ".o", "")
    components["ld"] = Ld("clang++", ".o", ".so", "")

    # set the parser and collect the args
    info = do_init(components)
    # set the limitation and output file name
    global limitation, output_name, input_name
    limitation = info["timeout"]
    output_name = info["output"]
    file_list = info["filename"]
    file_type = info["stage"]
    input_name = file_list[0]
    input_name = input_name.split("/")[-1].split(".")[0]
    if output_name == "":
        output_name = input_name + ".so"
    # set the compile options and tool path
    do_prepare(components, info, maple_root)
    # Select compile phase
    do_update(components, file_type)
    # check files one by one
    do_check(components, file_list)
    # compile accord with pre-install phases
    for phase in list(components.keys()):
        components[phase].build()
    logger.info("Compile Finished!")

if __name__ == "__main__":
    main()
