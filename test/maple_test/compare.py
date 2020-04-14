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
from functools import partial

from utils import complete_path, read_file
from utils import split_comment, filter_line

ASSERT_FLAG = "ASSERT"

SCAN_KEYWORDS = ["auto", "not", "next", "end"]
CMP_KEYWORDS = ["end", "not", "next", "full"]


class CompareError(Exception):
    pass


def main():
    opts = parse_cli()
    case_path = opts.case_path
    comment = opts.comment
    compare_object = opts.compare_object
    assert_flags = opts.assert_flag
    if not assert_flags:
        assert_flags.append(ASSERT_FLAG)
    content = compare_object.read()
    lines = content.splitlines(True)
    line_map = []
    start = 0
    for line in lines:
        length = len(line)
        line_map.append((start, start + length, line))
        start += length

    print("compare.py input:")
    print(indent(content, "\t", lambda line: True))
    print("compare.py input end\n")

    if compare_object.isatty():
        sys.stderr.write("ERROR: require compare objects, filepath or stdin \n")
        sys.exit(253)
    comment_lines = split_comment(comment, read_file(case_path))[1]
    compare_result = True
    print("Starting Match:")
    for assert_flag in assert_flags:
        assert_lines = [
            line for line in comment_lines if filter_line(line, assert_flag)
        ]
        if not assert_lines:
            print(
                "ASSERT flag: {}, No regex find, "
                "make sure you write the assert line".format(assert_flag)
            )
        match_pass = True
        start = 0
        for assert_line in assert_lines:

            pattern_flag, pattern = extract_pattern(assert_line, assert_flag)
            if not is_valid_pattern(pattern):
                match_pass = False
                break

            keywords = pattern_flag.split("-")
            valid_keywords = []
            assert_mode = keywords[0]
            match_func = None
            if assert_mode == "scan":
                match_func = regex_match
                valid_keywords = SCAN_KEYWORDS
            elif assert_mode == "cmp":
                match_func = cmp_match
                valid_keywords = CMP_KEYWORDS
            else:
                raise CompareError("scan mode: {} is not valid".format(assert_mode))
            for keyword in keywords[1:]:
                if keyword not in valid_keywords:
                    raise CompareError(
                        "keyword: {} is not valid for {}".format(keyword, assert_mode)
                    )
                if keyword == "auto":
                    match_func = partial(auto_regex_match, match_func=match_func)
                elif keyword == "not":
                    match_func = partial(not_match, match_func=match_func)
                elif keyword == "next":
                    match_func = partial(next_match, match_func=match_func)
                elif keyword == "end":
                    match_func = end_match
                elif keyword == "full":
                    match_func = full_match
            if "next" not in keywords and "end" not in keywords:
                start = 0
            result, start = match_func(content, line_map, pattern, start)
            print(
                " assert line: '{}', result: {}".format(
                    assert_line.split(":")[-1].strip(), result
                )
            )
            match_pass &= result
        compare_result &= match_pass
        if match_pass is False:
            print("Match End:")
            print(
                "ASSERT flag: {}, Compare Failed: {}".format(
                    " ".join(assert_flags), compare_result
                )
            )
            result = False
    if compare_result is True:
        print("Match End !!!")
        print(
            "ASSERT flag: {}, Compare Passed: {}".format(
                " ".join(assert_flags), compare_result
            )
        )
        return 0

    sys.exit(1)


def extract_pattern(line, flag):
    line_flag, pattern_line = line.lstrip().split(":", 1)
    if line_flag.strip() != flag:
        raise CompareError("Error: {} = {}".format(line_flag, flag))
    try:
        pattern_flag, raw_pattern = pattern_line.lstrip().split(" ", 1)
    except ValueError:
        pattern_flag = pattern_line.lstrip()
        raw_pattern = None
    return pattern_flag, raw_pattern


def is_valid_pattern(pattern):
    try:
        re.compile(pattern)
    except re.error:
        logging.error("Error pattern: {!r}".format(pattern))
        return False
    finally:
        return True


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
        "--compare_object",
        nargs="?",
        type=argparse.FileType("r"),
        default=sys.stdin,
        help="compare object, default stdin",
    )
    opts = parser.parse_args()
    return opts


def regex_match(content, line_map, pattern, start=0):
    matches = re.finditer(str(pattern), content, re.MULTILINE)
    end = 0
    for _, match in enumerate(matches, start=1):
        end = match.end()
        line_num = text_index_to_line_num(line_map, end)
        if line_num + 1 >= len(line_map):
            return True, end
        return True, line_map[line_num + 1][0]
    return False, start


def cmp_match(content, line_map, pattern, start=0):
    line_num = text_index_to_line_num(line_map, start)
    line = content.splitlines()[line_num]
    if line == pattern:
        return True, line_map[line_num][1]
    else:
        return False, start


def auto_regex_match(content, line_map, pattern, start=0, match_func=regex_match):
    pattern = r"\s+".join([re.escape(word.strip()) for word in pattern.strip().split()])
    return match_func(content, line_map, pattern, start)


def not_match(content, line_map, pattern, start=0, match_func=regex_match):
    result, end = match_func(content, line_map, pattern, start)
    return not result, end


def next_match(content, line_map, pattern, start=0, match_func=regex_match):
    return match_func(content, line_map, pattern, start)


def end_match(content, line_map, pattern, start=0, match_func=regex_match):
    line_num = text_index_to_line_num(line_map, start)
    if line_num < len(line_map):
        return False, start
    return True, start


def full_match(content, line_map, pattern, start=0, match_func=regex_match):
    pattern = pattern.encode("utf-8").decode("unicode_escape")
    if content != pattern:
        return False, start
    return True, start


def text_index_to_line_num(line_map, index):
    for line_num, line in enumerate(line_map):
        start, end, _ = line
        if start <= index < end:
            return line_num
    return line_num + 1


if __name__ == "__main__":
    logging.basicConfig(
        format="\t%(message)s", level=logging.DEBUG, stream=sys.stderr,
    )
    main()
