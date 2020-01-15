set_config("cc", "clang")
set_config("ld", "clang")

target("Editor")
    set_kind("binary")
    set_targetdir("build")
    add_linkdirs("lib")
    if is_os("macosx") then
       add_frameworks("CoreVideo", "IOKit", "Cocoa", "GLUT", "OpenGL")
       add_links("raylib")
    elseif is_os("windows") then
       add_links("raylib", "gdi32", "winmm")
       add_syslinks("pthread")
    end
    add_includedirs("src/scoreio", "src/editor", "src/others", "src/audio")
    add_files("src/scoreio/score.c","src/editor/main.c", "src/audio/sound.c")
    add_files("./res/thumb.o")
