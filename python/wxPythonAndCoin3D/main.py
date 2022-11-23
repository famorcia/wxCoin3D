# App contains the main of wx
import wx
from MyFrame import MyFrame
from pivy.coin import *

AppBaseClass = wx.App


class MyApp(AppBaseClass):
    def OnInit(self):
        SoDB.init()
        if SoDB.isInitialized() == False:
            SoDB.init
        SoNodeKit.init()
        SoInteraction.init()

        frame = MyFrame(None, "Simple wxPython App")
        self.SetTopWindow(frame)
        frame.Show(True)
        return True


if __name__ == "__main__":
    app = MyApp(redirect=False)
    app.MainLoop()
