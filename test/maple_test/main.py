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
maple test - Maple Tester


"""
import shutil
import time

from maple_test import configs
from maple_test.task import TestSuiteTask
from maple_test.utils import timer
from maple_test.run import TestError


@timer
def main():
    test_suite_config, running_config, log_config = configs.init_config()
    logger = configs.LOGGER

    test_paths = test_suite_config.get("test_paths")
    test_cfg = test_suite_config.get("test_cfg")
    cli_running_config = test_suite_config.get("cli_running_config")

    retry = configs.get_val("retry")
    result = []
    for test in test_paths:
        if test.exists():
            if not test_cfg:
                test_cfg = test / "test.cfg"
            try:
                task = TestSuiteTask(test, test_cfg, running_config, cli_running_config)
            except TestError as e:
                logger.info(e)
                continue
            if not task.task_set:
                continue
            for run_time in range(1, retry + 2):
                logger.info("Run {} times".format(run_time))
                task.run(configs.get_val("processes"))
                result.append(task.gen_summary([]))
        else:
            logger.info("Test path: {} does not exist, please check".format(test))

    output = configs.get_val("output")
    if output:
        if output.exists() and output.is_file():
            name = "{}_{}{}".format(output.stem, int(time.time()), output.suffix)
            logger.info(
                "result file: {} exists, will rename as: {}".format(output, name)
            )
            shutil.move(str(output), str(output.parent / name))
        logger.info("Save test result at: {}".format(output))
        with output.open("w") as f:
            for summary in result:
                f.write(summary)

    temp_dir = running_config.get("temp_dir")
    if configs.get_val("debug"):
        logger.debug("Keep temp file at %s", temp_dir)
    elif temp_dir.exists():
        logger.debug("remove temp_dir %s", temp_dir)
        shutil.rmtree(str(temp_dir))


if __name__ == "__main__":
    main()
