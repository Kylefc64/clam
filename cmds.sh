#!/bin/sh

# To use these shell command shortcuts, first run the following command from the root (clam) directory: source cmds.sh
# These command shortcuts must then be run from the root directory.

exe_path=bin/clam
test_path=test/run_tests.py
make_build_tree_path=build

gen-make() {
    if [ ! -e ${make_build_tree_path} ]
    then
        mkdir ${make_build_tree_path}
    fi

    cd ${make_build_tree_path}
    cmake -D CLIP_EXAMPLES=OFF -D CLIP_TESTS=OFF -D CLIP_X11_WITH_PNG=OFF ..
    cd ..
}

clean-make() {
    rm -rf ${make_build_tree_path}
    rm ${exe_path}
}

build() {
    gen-make
    cd ${make_build_tree_path}
    make
    cd ..
}

test() {
    build
    python3 ${test_path}
}
