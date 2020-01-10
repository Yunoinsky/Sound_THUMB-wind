add_rules("mode.debug", "mode.release")

target("Editor")
    set_kind("binary")
    set_targetdir("build")
    if is_os("macosx") then
       add_frameworks("CoreVideo", "IOKit", "Cocoa", "GLUT", "OpenGL")
    end
    add_includedirs("src/scoreio", "src/editor", "src/others", "src/audio")
    add_linkdirs("lib")
    add_links("raylib")
--    add_files("src/mus.c")
    add_files("src/scoreio/score.c","src/editor/main.c", "src/audio/sound.c")

target("Score")
    set_kind("binary")
    set_targetdir("build")
    add_files("src/scoreio/score.c")
    add_defines("TEST")
    


    
target("Game")
    set_kind("binary")
    set_targetdir("build")
    add_frameworks("CoreVideo", "IOKit", "Cocoa", "GLUT", "OpenGL")
    add_linkdirs("lib")
    add_links("raylib")

