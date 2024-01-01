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

class token:
    test = 0

class test:
    def __init__(self, name, test_runtime):
        token.test += 1

        self.name = name
        self.id = token.test
        self.test_runtime = test_runtime
        self.state = "not started" 

    def start(self):
        print("test: ", self.name, " id: ", self.id, " [starting]")

        self.state = "starting"
        self.state = self.test_runtime()

    def end(self):
        print("test: ", self.name, " id: ", self.id, " [{end_state}]".format(end_state = self.state))

import pyautogui

class auto_gui:
    screen_size = pyautogui.size()
    test_program_window_size = [1280, 720]
    test_program_window_pos = [0, 0]

    def update_scaled_mouse_pos():
        auto_gui.test_program_window_pos[0] = (auto_gui.screen_size[0] / 2) - (auto_gui.test_program_window_size[0] / 2)
        auto_gui.test_program_window_pos[1] = (auto_gui.screen_size[1] / 2) - (auto_gui.test_program_window_size[1] / 2)

    def set_mouse_centered():
        pyautogui.moveTo(auto_gui.screen_size[0] / 2, auto_gui.screen_size[1] / 2)

    def set_mouse_pos(x, y):
        auto_gui.update_scaled_mouse_pos()
        pyautogui.moveTo(auto_gui.test_program_window_pos[0] + x, auto_gui.test_program_window_pos[1] + y)

    def press_mouse(x, y, button):
        auto_gui.update_scaled_mouse_pos()
        pyautogui.click(x = auto_gui.test_program_window_pos[0] + x, y = auto_gui.test_program_window_pos[1] + y, button = button)
