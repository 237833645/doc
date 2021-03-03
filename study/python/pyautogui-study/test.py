#!/usr/local/bin/python3

import pyautogui
import time


def get_mouse_xy():
    time.sleep(1)
    try:
        for i in range(1000):
            # Get and print the mouse coordinates.
            x, y = pyautogui.position()
            positionStr = '1（X,Y）为：{},{}'.format(str(x).rjust(4), str(y).rjust(4))
            pix = pyautogui.screenshot().getpixel((x, y))
            positionStr += ' RGB:(' + str(pix[0]).rjust(3) + ',' + str(pix[1]).rjust(3) + ',' + str(pix[2]).rjust(
                3) + ')'
            print(positionStr)
            time.sleep(0.1)
    except:
        print('1111')


if __name__ == "__main__":
    print(get_mouse_xy())
