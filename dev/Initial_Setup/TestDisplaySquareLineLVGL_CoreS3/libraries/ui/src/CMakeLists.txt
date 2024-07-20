SET(SOURCES ui_StartUpScreen.c
    ui_MainScreen.c
    ui_ValidationScreen.c
    ui_EndScreen.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c)

add_library(ui ${SOURCES})
