from auto_gui_tools import test, auto_gui, pyautogui

import platform
import os
import time
import random

def init_test_program():
    test_program_path = os.getcwd()
    test_program_path = test_program_path[0:len(test_program_path) - len("\\autogui")]
    test_program_path = test_program_path + "\\build\\{os}".format(os = "win32" if platform.system() ==  "Windows" else "linux")

    os.system("cd {path} && start ekg-tests.exe".format(path = test_program_path))
    time.sleep(1)

def end_test_test_program():
    print("ending the tests sucessfully")
    auto_gui.press_mouse(x = 660, y = 690, button = "left")

def run_loaded_test(loaded_list_test):
    auto_gui.set_mouse_centered()

    for tests in loaded_list_test:
        tests.start()
        tests.end()

def textbox():
    auto_gui.press_mouse(200, 200, "left")
    
    time_count_ticks = 0
    time_out = 20

    pyautogui.write("oi", interval = 0.1)

    for x in range(200):
        pyautogui.press("backspace")

    return "complete"

if __name__ == "__main__":
    loaded_list_test = []
    loaded_list_test.append(test("textbox", textbox))

    init_test_program()
    run_loaded_test(loaded_list_test)
    end_test_test_program()