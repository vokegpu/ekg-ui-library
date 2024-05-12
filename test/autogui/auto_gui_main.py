"""
MIT License

Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

from auto_gui_tools import test, auto_gui, pyautogui

import platform
import os
import time
import random


def init_test_program():
    test_program_path = os.getcwd()
    test_program_path = test_program_path[0 : len(test_program_path) - len("\\autogui")]
    test_program_path = test_program_path + "\\build\\{os}".format(
        os="win32" if platform.system() == "Windows" else "linux"
    )

    os.system("cd {path} && start ekg-tests.exe".format(path=test_program_path))
    time.sleep(1)


def end_test_test_program():
    print("ending the tests sucessfully")
    auto_gui.press_mouse(x=660, y=690, button="left")


def run_loaded_test(loaded_list_test):
    auto_gui.set_mouse_centered()

    for tests in loaded_list_test:
        tests.start()
        tests.end()


def textbox():
    auto_gui.press_mouse(200, 200, "left")

    time_count_ticks = 0
    time_out = 20

    pyautogui.write("oi", interval=0.1)

    for x in range(200):
        pyautogui.press("backspace")

    return "complete"


if __name__ == "__main__":
    loaded_list_test = []
    loaded_list_test.append(test("textbox", textbox))

    init_test_program()
    run_loaded_test(loaded_list_test)
    end_test_test_program()
