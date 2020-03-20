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
from collections import defaultdict
from pathlib import Path

from maple_test import configs
from maple_test.run import run_commands, progress, TestError
from maple_test.test import Case, read_list
from maple_test.utils import (
    EXECUTABLE,
    COMPARE,
    PASS,
    NOT_RUN,
    UNRESOLVED,
    read_config,
    config_section_to_dict,
    ls_all,
)


class TaskConfig:
    def __init__(self, name, path, upper_config, user_config=None, user_env=None):
        if user_env is None:
            user_env = {}
        if user_config is None:
            user_config = {}
        if upper_config is None:
            self.internal_var = {}
            self.env = {}
            self.suffix_comments = {}
        else:
            self.internal_var = copy.deepcopy(upper_config.internal_var)
            self.env = copy.deepcopy(upper_config.env)
            self.suffix_comments = copy.deepcopy(upper_config.suffix_comments)
        if path.exists():
            self.name = name
        else:
            self.name = upper_config.name
        self.path = path
        self.update_sub_config()
        self.update_by_user_config(user_config, user_env)

    def update_by_user_config(self, user_config, user_env):
        self.internal_var.update(user_config)
        self.env.update(user_env)

    def update_sub_config(self):
        if self.path.exists():
            config = read_config(self.path)
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
        case_config["internal_var"]["f"] = str(case.path)
        case_config["internal_var"]["n"] = str(case.path.stem)
        case_config["internal_var"]["d"] = str(case.path.parent)
        return case_config


class TestSuiteTask:
    def __init__(self, test_path, cfg_path, running_config, cli_running_config=None):
        if cli_running_config is None:
            cli_running_config = {}
        user_test_cfg = cli_running_config.get("test_cfg")
        user_test_list = cli_running_config.get("test_list")
        user_config_set = cli_running_config.get("user_config_set")
        user_config = cli_running_config.get("user_config")
        user_env = cli_running_config.get("user_env")

        self.path = test_path
        self.cfg_path = user_test_cfg or cfg_path

        config = read_config(self.cfg_path)
        if config is None:
            raise TestError(
                "Test suite config path:{} not found, skip!!!!!".format(self.cfg_path)
            )
        try:
            self.name = config["description"]["title"].replace(" ", "")
        except KeyError:
            self.name = test_path.name
        self.suffix_comments = config_section_to_dict(config, "suffix")

        self.result = defaultdict(int)

        self.config_set = {}
        self.testlist_set = {}

        self._form_config(test_path, config, user_config_set, user_config, user_env)
        self._form_testlist(
            test_path,
            config,
            running_config["encoding"],
            user_config_set,
            user_test_list,
        )

        self.task_set = {}
        self.task_set_result = {}
        self._form_task_set(running_config)

    def _form_config(self, test_path, config, user_config_set, user_config, user_env):
        top_config = TaskConfig(self.name, self.cfg_path, None, user_config, user_env)
        if not user_config_set:
            self.config_set[self.name] = top_config
        for name, config_path in config_section_to_dict(config, "config-set").items():
            if not user_config_set or name == user_config_set:
                self.config_set[name] = TaskConfig(
                    name, test_path / config_path, top_config, user_config, user_env
                )

    def _form_testlist(
        self, test_path, config, encoding, user_config_set, user_test_list
    ):
        for name, testlist_path in config_section_to_dict(config, "testlist").items():
            if not user_config_set or name == user_config_set:
                if user_test_list is None:
                    self.testlist_set[name] = read_list(
                        test_path / testlist_path, encoding
                    )
                else:
                    self.testlist_set[name] = read_list(user_test_list, encoding)
        if self.testlist_set.get("default") is None:
            self.testlist_set["default"] = set("*"), set()
        if test_path.is_file():
            self.testlist_set["default"] = set("."), set()
        if (test_path / "testlist").exists():
            testlist = read_list(test_path / "testlist", encoding)
        else:
            testlist = self.testlist_set["default"]
        self.testlist_set[self.name] = testlist
        self.all_cases = {}
        self._search_list(test_path, encoding)

    def _search_list(self, test_path, encoding):
        suffixes = self.suffix_comments.keys()
        for name, testlist in self.testlist_set.items():
            self.testlist_set[name] = []
            include, exclude = testlist
            case_files = set()
            if test_path.is_file():
                case_files = [test_path]
            else:
                for glob_pattern in include:
                    for include_path in test_path.glob(glob_pattern):
                        case_files.update(ls_all(include_path, suffixes))
                for glob_pattern in exclude:
                    for exclude_path in test_path.glob(glob_pattern):
                        case_files -= set(ls_all(exclude_path, suffixes))
                case_files = [file.relative_to(test_path) for file in case_files]
            for case_file in case_files:
                case_name = str(case_file).replace(".", "_")
                comment = self.suffix_comments[case_file.suffix[1:]]
                if case_name in self.all_cases:
                    self.testlist_set[name].append(self.all_cases[case_name])
                else:
                    case = Case(case_file, test_path, comment, encoding,)
                    self.all_cases[case_name] = case
                    self.testlist_set[name].append(self.all_cases[case_name])

    def _form_task_set(self, running_config):
        for name in self.config_set:
            config = self.config_set.get(name)
            cases = self.testlist_set.get(name)
            if cases is None:
                cases = self.testlist_set.get("default")
            self.task_set[name] = []
            self.task_set_result[name] = defaultdict(int)
            for case in cases:
                self.task_set[name].append(SingleTask(case, config, running_config))
                self.task_set_result[name][NOT_RUN] += 1

    def serial_run_task(self):
        for tasks_name in self.task_set:
            for index, task in enumerate(self.task_set[tasks_name]):
                if task.result[0] == PASS:
                    continue
                self.task_set_result[tasks_name][task.result[0]] -= 1
                _, task.result = run_commands(
                    (tasks_name, index), task.commands, **task.running_config
                )
                status, _ = task.result
                self.task_set_result[tasks_name][status] += 1

    def parallel_run_task(self, process_num):
        multiprocessing.freeze_support()
        pool = multiprocessing.Pool(min(multiprocessing.cpu_count(), process_num))
        result_queue = []
        for tasks_name in self.task_set:
            for index, task in enumerate(self.task_set[tasks_name]):
                if not task.commands:
                    self.task_set_result[tasks_name][task.result[0]] -= 1
                    task.result = (UNRESOLVED, None)
                    self.task_set_result[tasks_name][UNRESOLVED] += 1
                    continue
                if task.result[0] == PASS:
                    continue
                result_queue.append(
                    pool.apply_async(
                        run_commands,
                        args=((tasks_name, index), task.commands,),
                        kwds=task.running_config,
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
        if process_num < 0:
            logger.error("The number of running processes needs to be greater than 1")
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

    def gen_brief_summary(self):
        total = sum(self.result.values())
        total_summary = "TestSuiteTask: {}, Total: {}, ".format(
            self.name, total
        ) + "".join(
            [
                "{}: {}, ".format(k, v)
                for k, v in sorted(
                    self.result.items(), key=lambda item: item[1], reverse=True
                )
            ]
        )
        task_set_summary = ""
        for tasks_name in self.task_set:
            total = sum(self.task_set_result[tasks_name].values())
            task_set_summary += (
                "\n  "
                + tasks_name
                + ", total: {}, ".format(total)
                + "".join(
                    [
                        "{}: {}, ".format(k, v)
                        for k, v in sorted(
                            self.task_set_result[tasks_name].items(),
                            key=lambda item: item[1],
                            reverse=True,
                        )
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
        brief_summary = self.gen_brief_summary()
        summary = "-" * 120
        summary += "\nTestSuite Path: {}\n".format(self.path)
        for tasks_name in self.task_set:
            for task in sorted(self.task_set[tasks_name], key=lambda task: task.name):
                result = task.result[0]
                if not print_type or task.result[0] in configs.get_val("print_type"):
                    summary += "  {}, Case: {}, Result: {}\n".format(
                        tasks_name, task.case_path, result
                    )
        summary += "\n" + brief_summary
        summary += "-" * 120
        return summary

    @staticmethod
    def _is_cli_mode(cli_running_config):
        return bool([x for x in cli_running_config.values() if x])


class SingleTask:
    def __init__(self, case, config, running_config):
        self.name = "{}_task/{}".format(config.name, case.name)
        self.path = Path("{}_task/{}".format(config.name, case.name))
        config = config.get_case_config(case)
        self.commands = []
        self._form_commands(case, config)
        env = config["env"]
        temp_dir = "{}_{}".format(self.path.name.replace(".", "_"), int(time.time()))
        self.work_dir = running_config["temp_dir"] / self.path.parent / temp_dir
        timeout = running_config["timeout"]
        self.running_config = {
            "work_dir": self.work_dir,
            "log_config": (running_config["log_config"], self.name),
            "timeout": timeout,
            "env": env,
        }
        self.case_path = case.relative_path
        self.prepare(case, self.work_dir, config)
        log_dir = (running_config.get("log_config").get("dir") / self.name).parent
        self.prepare_dir(log_dir)
        self.result = (NOT_RUN, None)

    def prepare(self, case, dest, config):
        dependence = case.dependence
        src_path = case.path
        src_dir = src_path.parent
        logger = configs.LOGGER
        if not src_path.exists():
            logger.debug("Source: {} is not existing.\n".format(src_path))
            return
        self.prepare_dir(dest)
        shutil.copy(str(src_path), str(dest))
        logger.debug("Copy {} => {}".format(src_path, dest))
        self.prepare_dependence(src_dir, dependence, dest, config)

    @staticmethod
    def prepare_dependence(src_dir, dependence, dest, config):
        logger = configs.LOGGER
        src_files = []
        for file in dependence:
            file = SingleTask._form_line(file, config)
            src_path = src_dir / file
            if src_path.exists():
                src_files.append(src_path)
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
                EXECUTABLE, COMPARE, shlex.quote(case.comment)
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
                    continue
                if line[end].isalnum() or line[end] == "_":
                    continue
                else:
                    line = line[:start] + value + line[end:]
        return line


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
        if end == len(raw_command):
            next_char = ""
        else:
            next_char = raw_command[end]
        if (
            prev_char.isalnum()
            or prev_char == "_"
            or next_char.isalnum()
            or next_char == "_"
        ):
            continue
        else:
            raw_command = raw_command[:start] + compare_cmd + raw_command[end:]
    return raw_command
