CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(wxGLWithCoin3D)

SET(CMAKE_CXX_STANDARD 11)

LINK_DIRECTORIES(/opt/wxWidgets-3.2.1_gtk/lib /opt/Coin-4.0.0/lib)

ADD_EXECUTABLE(wxGLWithCoin3D TestGLCanvas.cpp MyFrame.cpp main.cpp)

TARGET_COMPILE_DEFINITIONS(wxGLWithCoin3D PRIVATE
                           -D_FILE_OFFSET_BITS=64
                           -DWXUSINGDLL
                           -D__WXGTK__)

TARGET_INCLUDE_DIRECTORIES(wxGLWithCoin3D PRIVATE
                           /opt/wxWidgets-3.2.1_gtk/lib/wx/include/gtk2-unicode-3.2
                           /opt/wxWidgets-3.2.1_gtk/include/wx-3.2
                           /opt/Coin-4.0.0/include)

TARGET_LINK_LIBRARIES(wxGLWithCoin3D PRIVATE GL
                      wx_gtk2u_gl-3.2
                      wx_gtk2u_xrc-3.2
                      wx_gtk2u_html-3.2
                      wx_gtk2u_qa-3.2
                      wx_gtk2u_core-3.2
                      wx_baseu_xml-3.2
                      wx_baseu_net-3.2
                      wx_baseu-3.2
                      Coin
                      -pthread)

