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
