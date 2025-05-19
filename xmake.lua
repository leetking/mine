add_requires(
    "libsdl2",
    "libsdl2_image",
    "libsdl2_ttf"
)

set_project("mine")
set_version("0.0.2")

target("mine")
    set_kind("binary")
    set_languages("c11")
    add_rules("mode.debug", "mode.release")
    set_warnings("all", "error")
    add_files("src/*.c")
    add_defines("VERSION=\"v0.0.2\"")
    if is_plat("windows") then
	add_cflags("/utf-8")
        add_ldflags("/subsystem:windows")
        --add_ldflags("/subsystem:console")
    else
        add_ldflags("-mwindwos")
    end
    -- add_packages 添加依赖
    add_packages("libsdl2", "libsdl2_image", "libsdl2_ttf")
