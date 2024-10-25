install_scripts()

file(GLOB mac_files *.mac )
install(
    FILES ${mac_files}
    DESTINATION macros
)
