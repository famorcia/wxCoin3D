PROJECT(wxGLWithCoin3D)

SET(CMAKE_CXX_STANDARD 11)

ADD_EXECUTABLE(wxGLWithCoin3D TestGLCanvas.cpp MyFrame.cpp main.cpp)

TARGET_INCLUDE_DIRECTORIES(wxGLWithCoin3D PRIVATE
        /opt/Coin-4.0.0/include)

TARGET_LINK_LIBRARIES(wxGLWithCoin3D PRIVATE
        OpenGL::GL
        OpenGL::GLU
        ${COIN3D_LINK_LIBRARIES}
        ${wxWidgets_LIBRARIES})

