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
"""
store config file and command line config

"""
import argparse
import logging
import sys
from pathlib import Path

from maple_test.utils import ALL, ENCODING, BASE_DIR
from maple_test.utils import complete_path, read_config, get_config_value, is_relative

TEST_CONFIG = {}
LOGGER = None
LOG_CONFIG = {}
CASE_ENCODING = []


def parse_args():
    parser = argparse.ArgumentParser()
    test_framework_parser = parser.add_argument_group("Test FrameWork arguments")
    test_framework_parser.add_argument(
        "--cfg",
        default=Path(BASE_DIR / "maple_test.cfg"),
        type=complete_path,
        help="Test framework configuration file",
    )
    test_framework_parser.add_argument(
        "-j",
        type=bigger_than_one_integer,
        metavar="<mum>",
        dest="processes",
        default=1,
        help="Run <num> cases in parallel",
    )
    test_framework_parser.add_argument(
        "--retry",
        metavar="<num>",
        default=0,
        type=bigger_than_one_integer,
        help="Re-run unsuccessful test cases",
    )
    test_framework_parser.add_argument(
        "--output",
        metavar="<file>",
        type=complete_path,
        help="Store test result at <file>",
    )
    test_framework_parser.add_argument(
        "--debug", action="store_true", help="keep test temp file"
    )
    test_framework_parser.add_argument(
        "--fail_exit",
        action="store_true",
        help="Execute test framework with a non-zero exit code if any tests fail",
    )
    test_framework_parser.add_argument(
        "-p",
        type=str,
        dest="print_type",
        action="append",
        default=[],
        choices=ALL[:],
        help="Print test cases with specified results, "
        "-pPASS -pFAIL, to print all test case that failed or passed",
    )
    test_framework_parser.add_argument(
        "--progress",
        choices=["silent", "normal", "no_flush_progress"],
        default="normal",
        help="set progress type, silent: Don't show progress, "
        "normal: one line progress bar, update per second,"
        "no_flush_progress: print test progress per 10 seconds",
    )
    test_framework_parser.add_argument(
        "--dry_run",
        action="store_true",
        help="enable dry run, will generate test.sh under test case temp dir",
    )

    test_suite_parser = parser.add_argument_group("Test Suite arguments")
    test_suite_parser.add_argument(
        "test_paths", nargs="*", type=complete_path, help="Test suite path",
    )
    test_suite_parser.add_argument(
        "--test_cfg",
        metavar="<TEST_CFG_FILE>",
        type=complete_path,
        help="test suite config file, "
        "needed when run a single case or with --test_list",
    )
    test_suite_parser.add_argument(
        "--test_list",
        metavar="<TEST_LIST_FILE>",
        type=complete_path,
        default=None,
        help="testlist path for filter test cases",
    )
    test_suite_parser.add_argument(
        "-c",
        "--config_set",
        action="append",
        dest="user_config_set",
        default=[],
        metavar="config set path",
        help="Run a test set with the specified config set path",
        type=complete_path,
    )
    test_suite_parser.add_argument(
        "-C",
        "--config",
        dest="user_config",
        metavar="key=value",
        help="Add 'key' = 'val' to the user defined configs",
        type=str,
        action=StoreDictKeyPair,
        default={},
    )
    test_suite_parser.add_argument(
        "-E",
        "--env",
        dest="user_env",
        metavar="key=value",
        help="Add 'key' = 'val' to the user defined environment variable",
        action=StoreDictKeyPair,
        default={},
    )

    running_parser = parser.add_argument_group("Running arguments")
    running_parser.add_argument(
        "--temp_dir",
        metavar="<TEMP_DIR_PATH>",
        type=complete_path,
        default=None,
        help="Location for test execute. ",
    )
    running_parser.add_argument(
        "--timeout", type=float, dest="timeout", default=600, help="test case timeout"
    )
    running_parser.add_argument(
        "--encoding",
        action="append",
        default=[ENCODING],
        help="Specify the test case encoding format, default encoding is platform "
        "dependent, but any encoding supported by Python can be passed. "
        "Can specify multiple encoding formats.",
    )

    log_parser = parser.add_argument_group("Log arguments")
    log_parser.add_argument(
        "--log_dir",
        metavar="<LOG_DIR_FILE_PATH>",
        type=complete_path,
        default=None,
        help="Where to store test log",
    )
    log_parser.add_argument(
        "--log_level",
        "-l",
        type=get_level_name,
        default=None,
        help="set log level from: CRITICAL, ERROR, WARNING, INFO, DEBUG, NOTSET",
    )
    log_parser.add_argument(
        "--verbose", action="store_true", help="enable verbose output"
    )

    args = parser.parse_args()
    if args.test_list and not args.test_cfg:
        print("Error: When specify test list, need also specify test configure file\n")
        parser.print_help()
        sys.exit(0)

    test_framework_config = {
        "cfg": args.cfg,
        "processes": args.processes,
        "retry": args.retry,
        "output": args.output,
        "debug": args.debug,
        "fail_exit": args.fail_exit,
        "print_type": args.print_type,
        "progress": args.progress,
        "dry_run": args.dry_run,
    }

    test_suite_config = {
        "test_paths": args.test_paths or None,
        "test_cfg": args.test_cfg,
        "cli_running_config": {
            "test_list": args.test_list,
            "user_config_set": args.user_config_set,
            "user_config": args.user_config,
            "user_env": args.user_env,
        },
    }

    running_config = {
        "temp_dir": args.temp_dir,
        "timeout": args.timeout,
        "encoding": args.encoding,
    }

    log_config = {
        "dir": args.log_dir,
        "level": args.log_level,
        "verbose": args.verbose,
    }

    return test_framework_config, test_suite_config, running_config, log_config


def parser_maple_test_config_file(maple_test_cfg_file):
    raw_config = read_config(maple_test_cfg_file)
    test_paths = get_config_value(raw_config, "test-home", "dir")
    if test_paths:
        test_paths = test_paths.replace("\n", "").split(":")
    else:
        test_paths = []
    test_suite_config = {
        "test_paths": [BASE_DIR / path for path in test_paths if path],
    }
    log_config = {
        "dir": complete_path(
            BASE_DIR / Path(get_config_value(raw_config, "logging", "name"))
        ),
        "level": get_level_name(get_config_value(raw_config, "logging", "level")),
    }

    running_config = {
        "temp_dir": complete_path(
            BASE_DIR / Path(get_config_value(raw_config, "running", "temp_dir"))
        ),
    }

    return test_suite_config, running_config, log_config


def init_config():
    global TEST_CONFIG
    (
        TEST_CONFIG,
        cli_test_suite_config,
        cli_running_config,
        cli_log_config,
    ) = parse_args()

    (
        file_test_suite_config,
        file_running_config,
        file_log_config,
    ) = parser_maple_test_config_file(TEST_CONFIG.get("cfg"))

    cli_test_paths = cli_test_suite_config.get("test_paths") or []
    file_test_paths = file_test_suite_config.get("test_paths") or []

    for path1 in cli_test_paths:
        for path2 in file_test_paths:
            if is_relative(path1, path2):
                user_test_cfg = cli_test_suite_config.get("test_cfg")
                if not user_test_cfg:
                    cli_test_suite_config["test_cfg"] = complete_path(
                        path2 / "test.cfg"
                    )

    test_suite_config = merge_config(cli_test_suite_config, file_test_suite_config)
    running_config = merge_config(cli_running_config, file_running_config)
    log_config = merge_config(cli_log_config, file_log_config)

    if log_config.get("dir") is None:
        log_dir = complete_path(Path("./maple_test_log"))
        print(
            "No log dir find in cfg file and cli args, will create lod gir at: {}".format(
                log_dir
            )
        )
        log_config["dir"] = log_dir

    global CASE_ENCODING
    CASE_ENCODING = TEST_CONFIG.get("encoding")

    global LOGGER
    maple_test_log_config = log_config.copy()
    maple_test_log_config["verbose"] = True
    maple_test_log_config["level"] = min(20, maple_test_log_config.get("level"))
    stream_fmt = logging.Formatter("%(message)s")
    log_dir = maple_test_log_config.get("dir")
    if not log_dir.exists():
        log_dir.mkdir(parents=True, exist_ok=True)
    LOGGER = construct_logger(
        maple_test_log_config,
        "Maple_Test",
        stream_fmt=stream_fmt,
        stream_level=min(20, maple_test_log_config.get("level")),
    )
    LOGGER.info("Test log saved to {}".format(log_dir))
    running_config["log_config"] = log_config.copy()

    return test_suite_config, running_config, log_config


def merge_config(cli_config, maple_test_config):
    config = {}
    config.update(maple_test_config)
    for key, value in cli_config.items():
        if value is not None:
            config[key] = value
    return config


def get_val(key):
    global TEST_CONFIG
    return TEST_CONFIG.get(key)


class StoreDictKeyPair(argparse.Action):
    def __init__(self, option_strings, dest, nargs=None, **kwargs):
        self._nargs = nargs
        super().__init__(option_strings, dest, nargs=nargs, **kwargs)

    def __call__(self, parser, namespace, value, option_string=None):
        my_dict = getattr(namespace, self.dest)
        k = value.strip().split("=")[0].strip()
        v = "=".join(value.strip().split("=")[1:]).strip()
        my_dict[k] = v
        setattr(namespace, self.dest, my_dict)


def construct_logger(
    log_config,
    name,
    stream_fmt=None,
    file_fmt=None,
    stream_level=None,
    file_level=logging.DEBUG,
):
    name = str(name)
    log_dir = str(log_config["dir"])
    level = log_config["level"]
    verbose = log_config["verbose"]
    formatter = logging.Formatter(
        "%(asctime)s %(levelname)s %(name)s %(message)s", datefmt="%Y-%m-%d %H:%M:%S"
    )
    if stream_fmt is None:
        stream_fmt = formatter
    if file_fmt is None:
        file_fmt = formatter
    logger = logging.getLogger(name)
    if logger.handlers:
        return logger
    logger.setLevel(logging.DEBUG)

    handler1 = logging.StreamHandler()
    handler1.setFormatter(stream_fmt)
    if not verbose:
        level = logging.WARNING
    else:
        level = logging.DEBUG
    if stream_level:
        level = stream_level
    handler1.setLevel(level)
    logger.addHandler(handler1)

    handler2 = logging.FileHandler(filename="{}/{}.log".format(log_dir, name), mode="w")
    handler2.setLevel(file_level)
    handler2.setFormatter(file_fmt)
    logger.addHandler(handler2)

    logger.debug("Log file at: {}/{}.log".format(log_dir, name))
    return logger


def get_level_name(name: str):
    name = name.upper()
    return logging.getLevelName(name)


def bigger_than_one_integer(num):
    inum = int(num)
    if inum < 1:
        raise argparse.ArgumentTypeError("{} is not bigger than one".format(num))
    return inum
