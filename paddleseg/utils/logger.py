# Copyright (c) 2020 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import time

import paddle

levels = {0: 'ERROR', 1: 'WARNING', 2: 'INFO', 3: 'DEBUG'}
log_level = 2


def log(level=2, message="", textBrowser=None):
    if paddle.distributed.ParallelEnv().local_rank == 0:
        current_time = time.time()
        time_array = time.localtime(current_time)
        current_time = time.strftime("%Y-%m-%d %H:%M:%S", time_array)
        if log_level >= level:
            if textBrowser is not None:
                textBrowser.append(
                    "{} [{}]\t{}".format(current_time, levels[level],
                                         message).encode("utf-8").decode("latin1"))
                textBrowser.repaint()
            print(
                "{} [{}]\t{}".format(current_time, levels[level],
                                     message).encode("utf-8").decode("latin1"))
            sys.stdout.flush()


def debug(message="", textBrowser=None):
    log(level=3, message=message, textBrowser=textBrowser)


def info(message="", textBrowser=None):
    log(level=2, message=message, textBrowser=textBrowser)


def warning(message="", textBrowser=None):
    log(level=1, message=message, textBrowser=textBrowser)


def error(message="", textBrowser=None):
    log(level=0, message=message, textBrowser=textBrowser)
