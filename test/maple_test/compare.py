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
import logging
import re
import sys
from textwrap import indent

from utils import complete_path, read_file
from utils import split_comment, filter_line


def main():
    opts = parse_cli()
    case_path = opts.case_path
    comment = opts.comment
    compare_object = opts.compare_object
    assert_flags = opts.assert_flag
    if not assert_flags:
        assert_flags.append("ASSERT")
    content = compare_object.read()

    print("compare.py input:")
    print(indent(content, "\t", lambda line: True))
    print("compare.py input end\n")

    if compare_object.isatty():
        sys.stderr.write("ERROR: require compare objects, filepath or stdin \n")
        sys.exit(253)
    comment_lines = split_comment(comment, read_file(case_path))[1]
    result = True
    for assert_flag in assert_flags:
        assert_lines = [
            line for line in comment_lines if filter_line(line, assert_flag)
        ]
        if not assert_lines:
            print(
                "ASSERT flag: {}, No regex find, "
                "make sure you write the assert line".format(assert_flag)
            )
        passed = True
        for assert_line in assert_lines:
            found = False
            flag, pattern = extract_pattern(assert_line, assert_flag)
            if pattern is None:
                logging.error(
                    "ASSERT flag: {}, Failed Reason: "
                    "Not found valid match pattern".format(assert_flag)
                )
                sys.exit(1)
            try:
                re.compile(pattern)
            except re.error:
                passed = False
                logging.error(
                    "ASSERT flag: {}, Failed Reason: "
                    "Error pattern: {!r}".format(assert_flag, pattern)
                )
                break
            print(
                "ASSERT flag: {}, Match regex: {}, "
                "is not-scan: {}".format(assert_flag, pattern, not flag)
            )
            if pattern:
                found = is_match(pattern, content)
            if found == flag:
                passed = True
            else:
                passed = False
                logging.error(
                    "Failed Reason: regex: {}, is not-scan: {}, Matched: {}, exit".format(
                        pattern, not flag, found
                    )
                )
                break
        if passed is False:
            result = False
    if result is True:
        print(
            "ASSERT flag: {}, Compare Passed: {}".format(" ".join(assert_flags), result)
        )
        return 0
    sys.exit(1)


def extract_pattern(line, flag):
    line_flag = line.strip().split(":")[0].strip()
    if line_flag != flag:
        return None, None

    line = line.strip()[len(line_flag) + 1 :].strip().lstrip(":").strip()
    if line[:5] == "scan ":
        words = line[4:].strip().split()
        pattern = r"\s*".join([word.strip() for word in words])
        return True, pattern
    if line[:9] == "scan-not ":
        words = line[8:].strip().split()
        pattern = r"\s*".join([word.strip() for word in words])
        return False, pattern
    if line[:10] == "scan-auto ":
        words = line[9:].strip().split()
        pattern = r"\s*".join([re.escape(word.strip()) for word in words])
        return True, pattern
    return None, None


def is_match(pattern, test_str):
    if re.findall(pattern, test_str, re.MULTILINE):
        return True
    return False


def parse_cli():
    parser = argparse.ArgumentParser(prog="compare")
    parser.add_argument("--comment", help="Test case comment")
    parser.add_argument(
        "--assert_flag",
        help="Test case assert flag, default ASSERT",
        action="append",
        default=[],
    )
    parser.add_argument(
        "case_path", type=complete_path, help="Source path: read compare rules"
    )
    parser.add_argument(
        "compare_object",
        nargs="?",
        type=argparse.FileType("r"),
        default=sys.stdin,
        help="compare object, default stdin",
    )
    opts = parser.parse_args()
    return opts


if __name__ == "__main__":
    logging.basicConfig(
        format="\t%(message)s", level=logging.DEBUG, stream=sys.stderr,
    )
    main()
