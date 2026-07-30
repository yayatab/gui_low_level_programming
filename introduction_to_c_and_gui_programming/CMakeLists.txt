include(FetchContent)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)


add_executable(gtk_bin main.c)
add_executable(gtk_bin_2_tree tree_views.c)

target_link_libraries(gtk_bin PRIVATE PkgConfig::GTK3)
target_link_libraries(gtk_bin_2_tree PRIVATE PkgConfig::GTK3)

