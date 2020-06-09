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
import copy
import multiprocessing
import shlex
import shutil
import time
import platform
from collections import defaultdict, OrderedDict
from pathlib import Path

from maple_test import configs
from maple_test.run import run_commands, progress, TestError
from maple_test.test import Case, read_list
from maple_test.utils import (
    EXECUTABLE,
    COMPARE,
    PASS,
    FAIL,
    NOT_RUN,
    UNRESOLVED,
    DEFAULT_PRINT,
    OS_SEP,
)
from maple_test.utils import (
    read_config,
    config_section_to_dict,
    get_config_value,
    ls_all,
    complete_path,
    is_relative,
    quote,
)


class TaskConfig:
    def __init__(self, path, config, top_config, user_config=None, user_env=None):
        if top_config is None:
            name = path.name
            self.internal_var = {}
            self.env = {}
            self.suffix_comments = {}
        else:
            self.inherit_top_config(top_config)
            name = str(path.relative_to(top_config.path.parent)).replace(OS_SEP, "_")
        self.name = name.replace(".", "_")
        self.path = complete_path(path)
        self.base_dir = self.path.parent
        self.update_sub_config(config)
        self.update_by_user_config(user_config, user_env)

    def inherit_top_config(self, top_config):
        self.internal_var = copy.deepcopy(top_config.internal_var)
        self.env = copy.deepcopy(top_config.env)
        self.suffix_comments = copy.deepcopy(top_config.suffix_comments)

    def update_by_user_config(self, user_config, user_env):
        if user_config is None:
            user_config = {}
        if user_env is None:
            user_env = {}
        self.internal_var.update(user_config)
        self.env.update(user_env)

    def update_sub_config(self, config):
        if self.path.exists():
            self.internal_var.update(config_section_to_dict(config, "internal-var"))
            self.env.update(config_section_to_dict(config, "env"))
            self.suffix_comments.update(config_section_to_dict(config, "suffix"))
        else:
            print(
                "config file: {}, not exists, will use upper config".format(self.path)
            )

    def get_case_config(self, case):
        case_config = {
            "internal_var": copy.deepcopy(self.internal_var),
            "env": copy.deepcopy(self.env),
        }
        case_config["internal_var"]["f"] = str(case.path.name)
        case_config["internal_var"]["n"] = str(case.path.stem)
        return case_config

    def __repr__(self):
        return str(self.name)


class TestSuiteTask:
    def __init__(self, test_path, cfg_path, running_config, cli_running_config=None):
        if cli_running_config is None:
            cli_running_config = {}

        self.path = complete_path(test_path)
        self.cfg_path = cfg_path

        config = read_config(self.cfg_path)
        if config is None:
            raise TestError(
                "Test suite config path:{} not found, skip!!!!!".format(self.cfg_path)
            )
        try:
            self.name = config["description"]["title"].replace(" ", "")
        except KeyError:
            self.name = self.path.name
        self.suffix_comments = config_section_to_dict(config, "suffix")

        self.result = defaultdict(int)

        self.config_set = {}
        self.testlist_set = {}

        self.task_set = defaultdict(list)
        self.task_set_result = {}
        self.all_cases = {}
        self._form_task_set(running_config, cli_running_config)

    def _form_task_set(self, running_config, cli_running_config):
        logger = configs.LOGGER
        user_test_list = cli_running_config.get("test_list")
        user_config_set = cli_running_config.get("user_config_set")
        user_config = cli_running_config.get("user_config")
        user_env = cli_running_config.get("user_env")

        raw_top_config = read_config(self.cfg_path)
        top_config = TaskConfig(
            self.cfg_path, raw_top_config, None, user_config, user_env
        )
        top_dir = top_config.base_dir
        if user_test_list is None:
            top_testlist_path = self._get_testlist(raw_top_config, top_dir)
        else:
            top_testlist_path = top_dir / user_test_list

        if user_config_set:
            run_config_set = user_config_set
        else:
            run_config_set = [self.cfg_path]

        for cfg in run_config_set:
            if not cfg.exists():
                logger.error("Error: cfg file: {} not found, will skip".format(cfg))
                continue
            try:
                raw_config = read_config(cfg)
            except:
                logger.error(
                    "The found configuration file {} has an error and will "
                    "skip the test configuration file, please modify the "
                    "configuration file or ignore the message. ".format(cfg)
                )
                continue
            config = TaskConfig(cfg, raw_config, top_config, user_config, user_env)
            name = config.name
            base_dir = config.base_dir
            if cfg == self.cfg_path:
                testlist_path = top_testlist_path
            else:
                testlist_path = self._get_testlist(raw_config, base_dir)
            self.task_set_result[name] = OrderedDict(
                {PASS: 0, FAIL: 0, NOT_RUN: 0, UNRESOLVED: 0}
            )
            for case in self._search_list(base_dir, testlist_path):
                task = SingleTask(case, config, running_config)
                self.task_set[name].append(task)
                self.task_set_result[name][task.result[0]] += 1
        if sum([len(case) for case in self.task_set.values()]) < 1:
            logger.info(
                "Path %s not in testlist, be sure add path to testlist", str(self.path),
            )

    @staticmethod
    def _get_testlist(config, base_dir):
        testlist_path = get_config_value(config, "testlist", "path")
        if testlist_path is None:
            testlist_path = base_dir / "testlist"
        else:
            testlist_path = base_dir / testlist_path
        return testlist_path

    def _search_list(self, base_dir, testlist_path):
        logger = configs.LOGGER
        suffixes = self.suffix_comments.keys()
        include, exclude = read_list(testlist_path)
        case_files = set()
        cases = []
        all_test_case, exclude_test_case = self._search_case(
            include, exclude, base_dir, suffixes
        )
        case_files = set()
        for pattern in all_test_case:
            _cases = all_test_case[pattern]
            if _cases:
                case_files.update(_cases)
            else:
                logger.info(
                    "Testlist: {}, ALL-TEST-CASE: {} is contain not test case".format(
                        testlist_path, pattern
                    )
                )
        for pattern in exclude_test_case:
            _cases = exclude_test_case[pattern]
            if _cases:
                case_files -= _cases
            else:
                logger.info(
                    "Testlist: {}, EXCLUDE-TEST-CASE: {} is contain not test case".format(
                        testlist_path, pattern
                    )
                )

        if self.path.is_file():
            case_files = [self.path]
        else:
            case_files = [
                file.relative_to(self.path)
                for file in case_files
                if is_relative(file, self.path)
            ]
        for case_file in case_files:
            case_name = str(case_file).replace(".", "_")
            comment = self.suffix_comments[case_file.suffix[1:]]
            if case_name not in self.all_cases:
                case = Case(case_file, self.path, comment,)
                self.all_cases[case_name] = case
            cases.append(self.all_cases[case_name])
        return cases

    @staticmethod
    def _search_case(include, exclude, base_dir, suffixes):
        case_files = set()
        all_test_case = {}
        exclude_test_case = {}
        for glob_pattern in include:
            all_test_case[glob_pattern] = set()
            for include_path in base_dir.glob(glob_pattern):
                case_files.update(ls_all(include_path, suffixes))
                all_test_case[glob_pattern].update(ls_all(include_path, suffixes))
        for glob_pattern in exclude:
            exclude_test_case[glob_pattern] = set()
            for exclude_path in base_dir.glob(glob_pattern):
                case_files -= set(ls_all(exclude_path, suffixes))
                exclude_test_case[glob_pattern].update(ls_all(exclude_path, suffixes))
        return all_test_case, exclude_test_case

    def serial_run_task(self):
        for tasks_name in self.task_set:
            for index, task in enumerate(self.task_set[tasks_name]):
                if task.result[0] == PASS or task.result[0] == UNRESOLVED:
                    continue
                self.task_set_result[tasks_name][task.result[0]] -= 1
                _, task.result = run_commands(
                    (tasks_name, index),
                    task.result,
                    task.commands,
                    **task.running_config
                )
                if configs.get_val("fail_verbose"):
                    self.output_failed((_, task.result))
                status, _ = task.result
                self.task_set_result[tasks_name][status] += 1

    def output_failed(self, result):
        output_template = (
            "\n--------\n"
            "TestCase Failed: \n"
            "Name           : {name}\n"
            "CMD list       : \n--------\n"
            "{cmd_list}"
            "--------\n"
            "Output         : \n{cmd_output}"
            "--------\n"
        )

        cmd_list_template = "{0: <15}:{1}\n"

        cmd_template = (
            "\n"
            "CMD            : {cmd}\n"
            "Return code    : {return_code}\n"
            "Stdout         : {stdout}\n"
            "Stderr         : {stderr}\n"
        )

        postion, result = result
        case = self.task_set[postion[0]][postion[1]]
        output = {}
        output["name"] = str(case)
        output["cmd_list"] = ""
        output["cmd_output"] = ""
        if result[0] == PASS:
            return
        if result[0] != PASS and not isinstance(result[1], str):
            for cmd_result in result[1]:
                output["cmd_output"] += cmd_template.format(**cmd_result)
            for cmd in case.commands[: len(result[1]) - 1]:
                output["cmd_list"] += cmd_list_template.format(PASS, cmd)
            output["cmd_list"] += cmd_list_template.format(
                FAIL, case.commands[len(result[1]) - 1]
            )
            for cmd in case.commands[len(result[1]) :]:
                output["cmd_list"] += cmd_list_template.format(NOT_RUN, cmd)
        else:
            output["cmd_list"] = "Test case preparation failed, " + result[1] + "\n"
            output["cmd_output"] = output["cmd_list"]
        print(output_template.format(**output))

    def parallel_run_task(self, process_num):
        multiprocessing.freeze_support()
        pool = multiprocessing.Pool(min(multiprocessing.cpu_count(), process_num))
        result_queue = []
        if configs.get_val("fail_verbose"):
            callback_func = self.output_failed
        else:
            callback_func = None
        for tasks_name in self.task_set:
            for index, task in enumerate(self.task_set[tasks_name]):
                if task.result[0] == PASS or task.result[0] == UNRESOLVED:
                    continue
                result_queue.append(
                    pool.apply_async(
                        run_commands,
                        args=((tasks_name, index), task.result, task.commands,),
                        kwds=task.running_config,
                        callback=callback_func,
                    )
                )
        progress(result_queue, configs.get_val("progress"))
        pool.close()
        pool.join()

        result_queue = [result.get() for result in result_queue]
        for position, result in result_queue:
            tasks_name, index = position
            task = self.task_set[tasks_name][index]
            self.task_set_result[tasks_name][task.result[0]] -= 1
            task.result = result
            self.task_set_result[tasks_name][result[0]] += 1

    def run(self, process_num=1):
        logger = configs.LOGGER
        if process_num == 1:
            logger.debug("The number of running processes is 1, which will run serial")
            self.serial_run_task()
        else:
            logger.debug(
                "The number of running processes is {}, and will run in parallel".format(
                    process_num
                )
            )
            self.parallel_run_task(process_num)
        print_type = configs.get_val("print_type")
        for line in self.gen_summary(print_type).splitlines():
            logger.info(line)
        return self.result[FAIL]

    def gen_brief_summary(self, print_type):
        total = sum(self.result.values())
        result = copy.deepcopy(self.result)
        total -= result.pop(NOT_RUN)
        if UNRESOLVED not in print_type:
            total -= result.pop(UNRESOLVED)

        total_summary = "TestSuiteTask: {}, Total: {}, ".format(
            self.name, total
        ) + "".join(
            [
                "{}: {}, ".format(k, v)
                for k, v in sort_dict_items(result, index=1, reverse=True)
            ]
        )
        task_set_summary = ""
        for tasks_name in self.task_set:
            total = sum(self.task_set_result[tasks_name].values())
            task_result = copy.deepcopy(self.task_set_result[tasks_name])
            total -= task_result.pop(NOT_RUN)
            if UNRESOLVED not in print_type:
                total -= task_result.pop(UNRESOLVED)
            task_set_summary += (
                "\n  "
                + tasks_name
                + ", total: {}, ".format(total)
                + "".join(
                    [
                        "{}: {}, ".format(k, v)
                        for k, v in sort_dict_items(task_result, index=1, reverse=True)
                    ]
                )
            )
        return total_summary + task_set_summary + "\n"

    def gen_summary(self, print_type=None):
        self.result = defaultdict(int)
        for name in self.task_set_result:
            for status, num in self.task_set_result[name].items():
                self.result[status] += num
        if print_type is None:
            print_type = configs.get_val("print_type")
        brief_summary = self.gen_brief_summary(print_type)
        summary = "-" * 120
        summary += "\nTestSuite Path: {}\n".format(self.path)
        for tasks_name in self.task_set:
            for task in sorted(self.task_set[tasks_name], key=lambda task: task.name):
                result = task.result[0]
                if result in print_type or (not print_type and result in DEFAULT_PRINT):
                    summary += "  {}, Case: {}, Result: {}\n".format(
                        tasks_name, task.case_path, result
                    )
        summary += "\n" + brief_summary
        summary += "-" * 120
        return summary

    def gen_result(self):
        from maple_test.test import Result

        results = []
        for task_name in self.task_set:
            for task in self.task_set[task_name]:
                result = Result(
                    task.case_path,
                    self.cfg_path.parent.name,
                    task_name,
                    task.result[0],
                    task.commands,
                    task.result[-1],
                )
                results.append(result)
        return results

    def gen_xml_result(self, root):
        from xml.etree import ElementTree

        suite = ElementTree.SubElement(
            root,
            "testsuite",
            failures=str(self.result[FAIL]),
            tests=str(sum(self.result.values())),
            name="{} {}".format(self.name, self.path),
        )
        for result in sorted(self.gen_result()):
            result.gen_xml(suite)
        return suite

    def gen_json_result(self):
        json_result = OrderedDict()
        json_result["name"] = "{} {}".format(self.name, self.path)
        json_result["total"] = sum(self.result.values())
        for status in self.result:
            if status == NOT_RUN:
                continue
            json_result[status] = self.result[status]
        json_result["tests"] = []
        for result in sorted(self.gen_result()):
            json_result["tests"].append(result.gen_json_result())
        return json_result


class SingleTask:
    def __init__(self, case, config, running_config):
        self.name = "{}{}{}_{}".format(case.test_name, OS_SEP, case.name, config.name)
        self.path = Path(self.name)
        config = config.get_case_config(case)
        temp_dir = "{}_{}".format(self.path.name.replace(".", "_"), int(time.time()))
        self.work_dir = running_config["temp_dir"] / self.path.parent / temp_dir
        self.running_config = {
            "work_dir": self.work_dir,
            "log_config": (running_config["log_config"], self.name),
            "timeout": running_config["timeout"],
            "env": config["env"],
        }
        self.case_path = case.relative_path
        if case.commands:
            prepare_result = self.prepare(case, self.work_dir, config)
            self.result = (NOT_RUN, None)
            log_dir = (running_config.get("log_config").get("dir") / self.name).parent
            self.prepare_dir(log_dir)
            self.result = prepare_result
        else:
            self.result = (UNRESOLVED, (1, "", "No valid command statement was found."))

        self.commands = []
        if self.result[0] == NOT_RUN:
            self._form_commands(case, config)

    def prepare(self, case, dest, config):
        src_path = case.path
        logger = configs.LOGGER
        if not src_path.exists():
            err = "Source: {} is not existing.\n".format(src_path)
            logger.debug(err)
            return FAIL, err
        self.prepare_dir(dest)
        shutil.copy(str(src_path), str(dest))
        logger.debug("Copy {} => {}".format(src_path, dest))
        return self.prepare_dependence(src_path.parent, case.dependence, dest, config)

    def prepare_dependence(self, src_dir, dependence, dest, config):
        logger = configs.LOGGER
        src_files = []
        for file in dependence:
            file = SingleTask._form_line(file, config)
            src_path = src_dir / file
            if src_path.exists():
                src_files.append(src_path)
            else:
                err = "DEPENDENCE keyword error, file: {} NotFound".format(file)
                logger.debug(err)
                return FAIL, err
        src_files = set(src_files)
        for file in src_files:
            if file.is_file():
                shutil.copy(str(file), str(dest))
            else:
                name = file.name
                try:
                    shutil.copytree(str(file), str(dest / name))
                except:
                    pass
        if src_files:
            logger.debug("Copy {} => {}".format(src_files, dest))
        return NOT_RUN, None

    @staticmethod
    def prepare_dir(directory):
        logger = configs.LOGGER
        if not directory.exists():
            try:
                directory.mkdir(parents=True, exist_ok=True)
                logger.debug("Create directory: {}".format(directory))
            except FileExistsError as err:
                logger.debug(err)
                logger.debug(
                    "File: {} is not an existing non-directory file.".format(directory)
                )

    def _form_commands(self, case, config):
        for command in case.commands:
            command = self._form_line(command, config)
            compare_cmd = " {} {} --comment={} ".format(
                EXECUTABLE, COMPARE, quote(case.comment)
            )
            self.commands.append(format_compare_command(command, compare_cmd))

    @staticmethod
    def _form_line(line, config):
        for key, value in config.get("internal_var").items():
            end = 0
            while end < len(line):
                start = line.find("%{}".format(key), end)
                if start == -1:
                    break
                end = len(key) + start + 1
                if end == len(line):
                    line = line[:start] + value + line[end:]
                elif not line[end].isalnum() and line[end] != "_":
                    line = line[:start] + value + line[end:]
        return line

    def __repr__(self):
        return "{}".format(self.path)


def format_compare_command(raw_command, compare_cmd):
    end = 0
    while end < len(raw_command):
        start = raw_command.find("compare ", end)
        if start == -1:
            break
        end = start + len("compare ")
        if start == 0:
            prev_char = ""
        else:
            prev_char = raw_command[start - 1]
        if not prev_char.isalnum() and prev_char != "_":
            raw_command = raw_command[:start] + compare_cmd + raw_command[end:]
    return raw_command


def sort_dict_items(d, index=0, reverse=False):
    return sorted(d.items(), key=lambda item: item[index], reverse=reverse)
