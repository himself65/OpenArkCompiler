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

import configparser
import locale
import os
import sys
import timeit
from functools import wraps
from pathlib import Path

EXEC_FLAG = "EXEC"
ASSERT_FLAG = "ASSERT"
EXPECT_FLAG = "EXPECT"
DEPENDENCE_FLAG = "DEPENDENCE"
PASS = "PASS"
FAIL = "FAIL"
XFAIL = "XFAIL"
XPASS = "XPASS"
NOT_RUN = "NOT_RUN"
UNRESOLVED = "UNRESOLVED"
UNSUPPORTED = "UNSUPPORTED"
UNSUCCESSFUL = [FAIL, UNSUPPORTED, UNRESOLVED, NOT_RUN]
ALL = [PASS, FAIL, XFAIL, XPASS, UNSUPPORTED, UNRESOLVED]
DEFAULT_PRINT = [PASS, FAIL, XFAIL, XPASS, UNSUPPORTED]
RESULT = {
    PASS: 0,
    FAIL: 0,
    NOT_RUN: 0,
    UNRESOLVED: 0,
    XFAIL: 0,
    XPASS: 0,
}
BASE_DIR = Path(__file__).parent.absolute()

ENCODING = locale.getpreferredencoding(False)
OS_SEP = os.path.sep
EXECUTABLE = sys.executable
COMPARE = BASE_DIR / "compare.py"


def read_file(file_path):
    """Read files based on encoding and return all file lines

    :param file_path: Path
    :param encoding: str
    :return:
    """
    lines = []
    with file_path.open(encoding="utf-8") as file:
        all_lines = file.readlines()
    for line in all_lines:
        if line.strip():
            lines.append(line.strip())
    return lines


def read_config(file_path):
    """Read config file based on encoding and return test config"""
    if not file_path.exists():
        return None
    config = configparser.ConfigParser()
    config.optionxform = str
    config.read(str(file_path), encoding="utf-8")
    return config


def get_config_value(config, section, option):
    """Read config value from test config"""
    try:
        return config[section][option]
    except KeyError:
        return None


def config_section_to_dict(config, section):
    try:
        return {k: v for k, v in config.items(section)}
    except configparser.NoSectionError:
        return {}


def ls_all(path, suffix=None):
    """Output all files in a directory"""
    all_files = []
    _path = complete_path(path)
    if _path.is_file() and is_case(_path, suffix):
        return [_path]
    for name, _, files in os.walk(str(_path)):
        for file in files:
            if is_case(Path(name) / file, suffix):
                all_files.append(Path(name) / file)
    return all_files


def is_case(path, suffix):
    """Determine if it is a test case based on the suffix

    :param suffix: tuple
    :param path: Path
    :return:
    """
    if suffix is None:
        return True
    elif isinstance(suffix, str):
        return path.suffix[1:] == suffix
    return path.suffix[1:] in suffix


def complete_path(path):
    """Returns the canonical path of a path"""
    path = Path(path)
    if not path.exists():
        return Path(os.path.realpath(str(path)))
    return path.expanduser().resolve()


def filter_line(line, flag=None):
    """Returns the line starting with the flag"""
    if flag is None:
        return line
    line_flag = line.strip().split(":")[0].strip()
    if line_flag == flag:
        new_line = line.strip()[len(flag) + 1 :].strip().lstrip(":").strip()
        return new_line
    return None


def split_comment(comment, lines):
    """Split text based on comments"""
    comment_lines = []
    uncomment_lines = []
    comment_len = len(comment)
    for line in lines:
        if line.strip()[:comment_len] == comment:
            comment_lines.append(line.strip()[comment_len:])
        else:
            uncomment_lines.append(line)
    return uncomment_lines, comment_lines


def add_run_path(new_path):
    """Add path to PATH"""
    run_env = os.environ.copy()
    old_path = run_env.get("PATH")
    if old_path:
        run_env["PATH"] = old_path + ":" + new_path
    else:
        run_env["PATH"] = new_path
    return run_env


def timer(func):
    """Decorator that reports the execution time."""

    @wraps(func)
    def wrapper(*args, **kwargs):
        start = timeit.default_timer()
        result = func(*args, **kwargs)
        end = timeit.default_timer()
        print(
            "Function: {}, args: {}, kwargs: {}, Time Consuming: {}s\n".format(
                func.__name__, str(args), str(kwargs), end - start
            )
        )
        return result

    return wrapper


def is_relative(path1, path2):
    """Is path1 relative to path2"""
    _p1 = complete_path(path1)
    _p2 = complete_path(path2)
    try:
        _p1.relative_to(_p2)
    except ValueError:
        return 0
    return 1


def merge_result(multi_results):
    for result in multi_results:
        if result in UNSUCCESSFUL:
            return result
    return PASS
