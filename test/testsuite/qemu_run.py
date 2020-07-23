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
import os
import shlex
import subprocess
import sys
import time
from collections import OrderedDict
from textwrap import indent

sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
from maple_test.utils import ENCODING, complete_path, add_run_path

EXIT_CODE = 0


class MapleRunError(Exception):
    pass


def run(cmd, work_dir, timeout):
    process_command = subprocess.Popen(
        cmd,
        shell=True,
        cwd=str(work_dir),
        env=add_run_path(str(work_dir)),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        close_fds=True,
    )
    try:
        com_out, com_err = process_command.communicate(timeout=timeout)
    except subprocess.CalledProcessError as err:
        raise err
    else:
        return_code = process_command.returncode
        com_out = com_out.decode(ENCODING, errors="ignore")
        com_err = com_err.decode(ENCODING, errors="ignore")
        return return_code, com_out, com_err
    finally:
        process_command.terminate()


def construct_qemu_cmd(execute_cmd, execute_option):
    maple_root = os.environ.get("MAPLE_ROOT")
    if maple_root is None:
        logging.error("Not found system environment : MAPLE_ROOT")
        sys.exit(1)
    execute_option["execute_files"] = ":".join(
        [str(file) for file in execute_option["execute_files"]]
    )
    LIBZ_SO = maple_root + "/output/ops/third-party"
    RUNTIME_SO = maple_root + "/output/ops/host-x86_64-" + execute_option["mrt_type"]
    APP_SO = os.getcwd()
    MPLSH = maple_root + "/output/ops/mplsh"
    execute_cmd["run_case"] = (
        "/usr/bin/qemu-aarch64 "
        "-L /usr/aarch64-linux-gnu -E LD_LIBRARY_PATH={LIBZ_SO}:{RUNTIME_SO}:{APP_SO} "
        "{MPLSH} -Xbootclasspath:libcore-all.so -cp {execute_files} "
        "{execute_class} {execute_args}".format(
            LIBZ_SO=LIBZ_SO, RUNTIME_SO=RUNTIME_SO, APP_SO=APP_SO,
            MPLSH=MPLSH, **execute_option
        )
    )


def parse_cli():
    parser = argparse.ArgumentParser(prog="qemu_run")
    parser.add_argument(
        "--run_type",
        default="aarch64",
        choices=["aarch64"],
        help="run type",
    )

    parser.add_argument(
        "execute_file", metavar="<file1>[:file2:file3...]", help="execute file, ",
    )
    parser.add_argument("execute_class", help="execute class")
    parser.add_argument("--execute_args", dest="execute_args", default="", help="execute args")
    parser.add_argument(
        "--timeout", help="run test case timeout", type=float, default=None
    )
    parser.add_argument(
        "--mrt_type",
        dest="mrt_type",
        default="OPS_O0",
        choices=["OPS_O0", "OPS_O2"],
        help="Add mrt type to the extra option",
    )

    connection_options = parser.add_argument_group("Script options")
    connection_options.add_argument(
        "--verbose", action="store_true", dest="verbose", help="enable verbose output",
    )

    opts = parser.parse_args()
    return opts


def main():
    opts = parse_cli()
    run_type = opts.run_type

    parser = shlex.shlex(opts.execute_file)
    parser.whitespace = ":"
    parser.whitespace_split = True
    source_files = [complete_path(file) for file in parser]

    execute_class = opts.execute_class
    execute_args = opts.execute_args
    mrt_type = opts.mrt_type
    timeout = opts.timeout

    execute_option = {
        "execute_files": source_files,
        "execute_class": execute_class,
        "execute_args": execute_args,
        "mrt_type": mrt_type,
        "timeout": timeout,
    }

    logging.basicConfig(
        format="\t%(asctime)s %(message)s",
        datefmt="%H:%M:%S",
        level=logging.DEBUG if opts.verbose else logging.INFO,
        stream=sys.stderr,
    )

    execute_cmd = OrderedDict()
    execute_cmd["run_case"] = None

    if run_type == "aarch64":
        construct_qemu_cmd(execute_cmd, execute_option)
    else:
        logging.error("Not support run type: {}".format(run_type))
    run_case(execute_cmd, ".", timeout)


def run_case(execute_cmd, work_dir, timeout):
    for stage, cmd in execute_cmd.items():
        run_cmd(stage, cmd, work_dir, timeout)


def run_cmd(stage, cmd, work_dir, timeout):
    return_code, com_out, com_err = run(cmd, work_dir, timeout)
    logging.debug("execute command: %s", cmd)
    logging.debug("execute return code: %d", return_code)
    logging.debug("execute out: \n%s", indent(com_out, "\t", lambda line: True))
    logging.debug("execute error: \n%s", indent(com_err, "\t", lambda line: True))
    global EXIT_CODE
    EXIT_CODE = return_code
    print(com_out, end="")
    print(com_err, file=sys.stderr, end="")
    if return_code != 0:
        logging.error("execute command: %s", cmd)
        logging.error("execute return code: %d", return_code)
        logging.error("execute out: \n%s", indent(com_out, "\t", lambda line: True))
        logging.error(
            "execute error: \n%s", indent(com_err, "\t", lambda line: True)
        )
        reason = "Maple run stage: {} failed at command: {}, reason: {}".format(
            "run_case".upper(), cmd, com_err
        )
        raise MapleRunError(reason)


if __name__ == "__main__":
    try:
        main()
    except MapleRunError as e:
        sys.exit(EXIT_CODE)
