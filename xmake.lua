add_rules("mode.debug", "mode.release")

target("pscxx")
    set_kind("binary")
    add_files("./*.cpp")
    add_includedirs(".")