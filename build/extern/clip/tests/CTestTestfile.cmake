# CMake generated Testfile for 
# Source directory: /home/kyle/repos/clam/extern/clip/tests
# Build directory: /home/kyle/repos/clam/build/extern/clip/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(clip_text_tests "/home/kyle/repos/clam/build/extern/clip/tests/clip_text_tests")
set_tests_properties(clip_text_tests PROPERTIES  RUN_SERIAL "TRUE")
add_test(clip_user_format_tests "/home/kyle/repos/clam/build/extern/clip/tests/clip_user_format_tests")
set_tests_properties(clip_user_format_tests PROPERTIES  RUN_SERIAL "TRUE")
add_test(clip_image_tests "/home/kyle/repos/clam/build/extern/clip/tests/clip_image_tests")
set_tests_properties(clip_image_tests PROPERTIES  RUN_SERIAL "TRUE")
