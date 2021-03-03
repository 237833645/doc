#!/usr/local/bin/python3
import time

import pyautogui


def get_mouse_xy():
    time.sleep(1)
    for i in range(1000):
        # Get and print the mouse coordinates.
        x, y = pyautogui.position()
        positionStr = '1（X,Y）为：{},{}'.format(str(x).rjust(4), str(y).rjust(4))
        pix = pyautogui.screenshot().getpixel((x, y))
        positionStr += ' RGB:(' + str(pix [0]).rjust(3) + ',' + str(pix [1]).rjust(3) + ',' + str(pix [2]).rjust(
            3) + ')'
        print(positionStr)
        time.sleep(0.1)


def get_screen():
    time.sleep(1)
    try:
        img = pyautogui.screenshot().save("img.png")
        img1 = pyautogui.screenshot(region=(50, 50, 500, 500)).save("img2.png")
    except ImportError:
        print("PyAutoGUIException")


if __name__ == "__main__":
    # print(pyautogui.screenshot().height)
    # print(pyautogui.screenshot().width)

    time.sleep(2)

    # print(pyautogui.position())

    # pyautogui.moveTo(x=1200, y=700, duration=1.0, logScreenshot=True)

    get_screen()

    # print(time.localtime())
    # pyautogui.screenshot(region=(0, 1000, 460*2, 795*2)).save("1.png")
    # print(time.localtime())

    # print(time.localtime())
    # x, y = pyautogui.locateOnScreen("1.png", region=(0, 1000, 460*2, 795*2))

    # pyautogui.moveTo(288, 527, duration=2.0)

    # pyautogui.moveTo(x, y)

    # print(time.localtime())

    # print(get_mouse_xy())


# 1280   2560

# 800    1600