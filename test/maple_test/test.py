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
import shlex
from functools import total_ordering

from maple_test.utils import PASS, EXEC_FLAG, EXPECT_FLAG, DEPENDENCE_FLAG
from maple_test.utils import read_file
from maple_test.utils import split_comment, filter_line
from maple_test.utils import FAIL, UNRESOLVED


class Case:
    def __init__(self, path, test_path, comment):
        if path != test_path:
            self.name = str(path).replace(".", "_")
            self.path = test_path / path
            self.test_name = test_path.name
        else:
            self.name = "{}_{}".format(path.parent.name, path.name).replace(".", "_")
            self.path = path
            self.test_name = path.parent.name
        self.test_path = test_path
        self.relative_path = path
        self.comment = comment
        try:
            _, comment_lines = split_comment(comment, read_file(self.path),)
        except UnicodeDecodeError as e:
            print(e)
            self.commands = []
            self.expect = []
            self.dependence = {}
        else:
            self.commands = extract_commands(comment_lines)
            self.expect = extract_expect(comment_lines)
            self.dependence = extract_dependence(comment_lines)

    def __repr__(self):
        return str(self.relative_path)


def extract_expect(comment_lines):
    expect_line = [filter_line(line, EXPECT_FLAG) for line in comment_lines]
    expect_line = [line for line in expect_line if line]
    if not expect_line:
        return PASS
    return expect_line[-1]


def extract_dependence(comment_lines):
    support_separartor = ",; "
    dependence = []
    for line in comment_lines:
        line = filter_line(line, DEPENDENCE_FLAG)
        if not line:
            continue
        parser = shlex.shlex(line)
        parser.whitespace += support_separartor
        parser.whitespace_split = True
        dependence += list(parser)
    return set(dependence)


def extract_commands(comment_lines):
    commands = []
    flag = False
    merge_command = ""
    for command in comment_lines:
        command = filter_line(command, EXEC_FLAG)
        if not command:
            continue
        if command.strip()[-1] == "\\":
            flag = True
            merge_command += "{} ".format(command.strip()[:-1])
        else:
            if flag:
                merge_command += "{} ".format(command)
            flag = False
            if merge_command == "":
                commands.append(command)
            else:
                commands.append(merge_command)
                merge_command = ""
    if not commands and merge_command.strip():
        commands.append(merge_command)
    return commands


def read_list(path):
    if not path.exists():
        return {"*"}, {}
    valid_lines, _ = split_comment("#", read_file(path))
    include_flag = "[ALL-TEST-CASE]"
    exclude_flag = "[EXCLUDE-TEST-CASE]"
    case_list = set()
    exclude_case_list = set()
    is_exclude = False
    for line in valid_lines:
        if line.find(include_flag) != -1:
            is_exclude = False
        elif line.find(exclude_flag) != -1:
            is_exclude = True
        elif is_exclude:
            exclude_case_list.add(line)
        else:
            case_list.add(line)
    if not case_list:
        case_list = {"*"}
    return case_list, exclude_case_list


@total_ordering
class Result:
    def __init__(self, case, task, cfg, status, commands, commands_result):
        self.case = case
        self.task = task
        self.cfg = cfg
        self.time = None
        self.status = status
        self.commands = commands
        self.commands_result = commands_result

    def gen_xml(self, root):
        from xml.etree import ElementTree

        case = ElementTree.SubElement(
            root,
            "testcase",
            name=str(self.case),
            classname="{}.{}".format(self.task, self.cfg),
        )

        if self.status == FAIL:
            failure = ElementTree.SubElement(case, "failure")
            failure.text = "List of commands:\n"
            for cmd in self.commands:
                failure.text += "EXEC: {}\n".format(cmd)
            failure.text += "----\n"
            failure.text += self.command_result_to_text(self.commands_result[-1])
        elif self.status == UNRESOLVED:
            skipped = ElementTree.SubElement(case, "skipped")
            skipped.text = "No valid command statement was found."

    def command_result_to_text(self, result):
        text = "EXEC: {}\n".format(result.get("cmd"))
        text += "Return code: {}\n".format(result.get("return_code"))
        text += "Stdout: \n{}\n".format(result.get("stdout"))
        text += "Stderr: \n{}\n".format(result.get("stderr"))
        return text

    def gen_json_result(self):
        from collections import OrderedDict

        result = OrderedDict()
        result["name"] = "{}/{} :: {}".format(self.task, self.case, self.cfg)
        result["result"] = self.status
        result["commands"] = self.commands
        result["output"] = self.commands_result
        return result

    def __lt__(self, other):
        return self.case < other.case
