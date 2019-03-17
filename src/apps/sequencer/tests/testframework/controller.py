import os

buttonMap = {
    "step9" : 0,
    "step10" : 1,
    "step11" : 2,
    "step12" : 3,
    "step13" : 4,
    "step14" : 5,
    "step15" : 6,
    "step16" : 7,
    "step1" : 8,
    "step2" : 9,
    "step3" : 10,
    "step4" : 11,
    "step5" : 12,
    "step6" : 13,
    "step7" : 14,
    "step8" : 15,
    "track1" : 16,
    "track2" : 17,
    "track3" : 18,
    "track4" : 19,
    "track5" : 20,
    "track6" : 21,
    "track7" : 22,
    "track8" : 23,
    "play" : 24,
    "clock" : 25,
    "patt" : 26,
    "perf" : 27,
    "prev" : 28,
    "next" : 29,
    "shift" : 30,
    "page" : 31,
    "f1" : 32,
    "f2" : 33,
    "f3" : 34,
    "f4" : 35,
    "f5" : 36,
}

pageButtonMap = {
    "clock" : "clock",
    "patt" : "patt",
    "perf" : "perf",
    "project" : "track1",
    "layout" : "track2",
    "routing" : "track3",
    "midiout" : "track4",
    "userscale" : "track5",
    "system" : "track8",
    "steps" : "step1",
    "sequence" : "step2",
    "track" : "step3",
    "song" : "step4",
    "monitor" : "step8",
    "overview" : "prev"
}

class Controller:
    def __init__(self, simulator):
        self._simulator = simulator
        self._screenshotDir = "."

    def wait(self, ms = 100):
        self._simulator.wait(ms)
        return self

    def down(self, button):
        self._simulator.setButton(buttonMap[button], True)
        return self

    def up(self, button):
        self._simulator.setButton(buttonMap[button], False)
        return self

    def press(self, button, pre = 10, post = 10):
        self.down(button)
        self.wait(pre)
        self.up(button)
        self.wait(post)
        return self

    def selectPage(self, page):
        self.down("page")
        self.wait(10)
        self.press(pageButtonMap[page])
        self.up("page")
        self.wait(10)
        return self

    # def contextAction(self, )

    def downEncoder(self):
        self._simulator.setEncoder(True)
        return self

    def upEncoder(self):
        self._simulator.setEncoder(False)
        return self

    def pressEncoder(self, pre = 10, post = 10):
        self.downEncoder()
        self.wait(pre)
        self.upEncoder()
        self.wait(post)
        return self

    def encoder(self):
        self.pressEncoder()
        return self

    def rotateEncoder(self, count):
        for i in range(abs(count)):
            self._simulator.rotateEncoder(count)
            self.wait(10)
        return self

    def left(self):
        self.rotateEncoder(-1)
        return self

    def right(self):
        self.rotateEncoder(1)
        return self

    def adc(self, channel, voltage):
        self._simulator.setAdc(channel, voltage)
        return self

    def midi(self, port, message):
        self._simulator.sendMidi(port, message)
        return self

    def screenshotDir(self, path):
        self._screenshotDir = path
        return self

    def screenshot(self, filename):
        self.wait(50)
        (name, ext) = os.path.splitext(filename)
        filename = os.path.join(self._screenshotDir, name + ".png")
        self._simulator.screenshot(filename)
        return self
