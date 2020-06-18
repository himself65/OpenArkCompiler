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
from utils import split_comment, filter_line, escape

ASSERT_FLAG = "ASSERT"
EXPECTED_FLAG = "EXPECTED"
EXPECTED_REGEX = r"{line_num}.*\:.*"

SCAN_KEYWORDS = ["auto", "not", "next", "end"]
CMP_KEYWORDS = ["end", "not", "next", "full"]
EXPECTED_KEYWORDS = ["scan", "scan-not"]


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
    expected_flags = opts.expected_flag
    if not expected_flags:
        expected_flags.append(EXPECTED_FLAG)

    content = compare_object.read()
    content_line_map = gen_line_map(content)

    print("compare.py input:")
    print(indent(content, "\t", lambda line: True))
    print("compare.py input end\n")

    if compare_object.isatty():
        sys.stderr.write("ERROR: require compare objects, filepath or stdin \n")
        sys.exit(253)
    compare_line_regex = gen_compare_regex(comment, assert_flags, expected_flags)
    compare_lines = extract_compare_lines(case_path, compare_line_regex)

    compare_result = True
    start = 0
    print("Starting Compare:")
    for compare_line in compare_lines:
        output_line_num = text_index_to_line_num(content_line_map, start)

        compare_line, line_num = compare_line
        flag, compare_pattern = split_compare_line(compare_line)
        pattern_flag, pattern = split_pattern_line(compare_pattern)

        info = ""
        if flag.strip() in assert_flags:
            info = "It's a assert, "
        elif flag.strip() in expected_flags:
            pattern = EXPECTED_REGEX.format(line_num=line_num) + pattern
            if pattern_flag.strip() not in EXPECTED_KEYWORDS:
                raise CompareError(
                    "Unsupport expected keywords: {!r}".format(pattern_flag)
                )
        else:
            raise CompareError("Unsupport flag: {!r}".format(flag))

        keywords = pattern_flag.split("-")
        match_func = gen_match_func(keywords)
        if "next" not in keywords and "end" not in keywords:
            start = 0
        result, start = match_func(content, content_line_map, pattern, start)
        info += "flag: {}, pattern: {} , result: {}, matached at output line: {}".format(
            pattern_flag, pattern, result, output_line_num
        )
        print(info)

        if result is False:
            print("Match End!, Compare Failed")
        compare_result &= result

    if compare_result is True:
        print("Match End !!!")
        print("Compare Passed: {}".format(compare_result))
        return 0
    sys.exit(1)


def split_compare_line(compare_line):
    if len(compare_line.lstrip().split(":", 1)) < 2:
        print(
            "Please check compare line, found compare flag but no actual compare content!!!"
        )
        raise CompareError(
            "Please check compare line, found compare flag but no actual compare content!!!"
        )
    else:
        return compare_line.lstrip().split(":", 1)


def split_pattern_line(compare_pattern):
    try:
        pattern_flag, pattern = compare_pattern.lstrip().split(" ", 1)
    except ValueError:
        pattern_flag = compare_pattern.lstrip()
        pattern = ""
    return pattern_flag, pattern


def gen_match_func(keywords):
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
    return match_func


def is_valid_pattern(pattern):
    try:
        re.compile(pattern)
    except re.error:
        logging.error("Error pattern: {!r}".format(pattern))
        return False
    except TypeError:
        logging.error(type(pattern), repr(pattern))
    return True


def regex_match(content, line_map, pattern, start=0):
    pattern = r"\s+".join([word for word in pattern.split()])
    if not is_valid_pattern(pattern):
        raise CompareError("Not valid pattern: {!r}".format(pattern))
    matches = re.finditer(str(pattern), content, re.MULTILINE)
    end = 0
    for _, match in enumerate(matches, start=1):
        end = match.end()
        line_num = text_index_to_line_num(line_map, end)
        if line_num + 1 >= len(line_map):
            return True, end
        return True, line_map[line_num] + 1
    return False, start


def cmp_match(content, line_map, pattern, start=0):
    line_num = text_index_to_line_num(line_map, start)
    line = content.splitlines()[line_num]
    if line == pattern:
        return True, line_map[line_num] + 1
    else:
        return False, start


def auto_regex_match(content, line_map, pattern, start=0, match_func=regex_match):
    pattern = r"\s+".join([re.escape(word) for word in pattern.split()])
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


def gen_line_map(text):
    regex = ".*\n?"
    line_map = []
    for match in re.finditer(regex, text):
        line_map.append(match.end())
    return line_map


def text_index_to_line_num(line_map, index):
    for line_num, end in enumerate(line_map):
        if end >= index:
            return line_num
    return line_num + 1


def gen_compare_regex(comment, assert_flags, expected_flag):
    regex = ""
    for flag in expected_flag:
        excepted_regex = r"(?:{comment}\s*)({flag}[\t ]*\:[\t ]*.*$)".format(
            comment=escape("\\$()*+.[]?^{}|", comment), flag=flag
        )
        if regex != "":
            regex = "{}|{}".format(regex, excepted_regex)
        else:
            regex = excepted_regex
    for flag in assert_flags:
        assert_regex = r"(?:^[\t ]*{comment}\s*)({flag}[\t ]*\:[\t ]*.*$)".format(
            comment=escape("\\$()*+.[]?^{}|", comment), flag=flag
        )
        if regex != "":
            regex = "{}|{}".format(regex, assert_regex)
        else:
            regex = assert_regex
    return regex


def extract_compare_lines(file_path, regex):
    with file_path.open(encoding="utf-8") as f:
        content = f.read()
    matches = re.finditer(regex, content, re.MULTILINE)

    compare_lines = []
    end_regex = ".*\n?"
    line_map = []
    for match in re.finditer(end_regex, content):
        line_map.append(match.end())
    for match in matches:
        for group_num in range(0, len(match.groups())):
            group_num = group_num + 1
            if match.group(group_num) is None:
                continue
            for line_num, end in enumerate(line_map):
                if end > match.start(group_num):
                    compare_lines.append((match.group(group_num), line_num + 1))
                    break
    return compare_lines


def parse_cli():
    parser = argparse.ArgumentParser(prog="compare.py")
    parser.add_argument("--comment", help="Test case comment")
    parser.add_argument(
        "--assert_flag",
        help="Test case assert flag, default ASSERT",
        action="append",
        default=[],
    )
    parser.add_argument(
        "--expected_flag",
        help="Test case expected flag for compile, default EXCEPTED",
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


if __name__ == "__main__":
    logging.basicConfig(
        format="\t%(message)s", level=logging.DEBUG, stream=sys.stderr,
    )
    main()
