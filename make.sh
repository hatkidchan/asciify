#!/bin/sh
CC_INCLUDES="-I./include"
CC_FLAGS="-pedantic -Wall -Wextra"
CC_FLAGS="$CC_FLAGS -Wno-unused-but-set-variable";
CC_FLAGS="$CC_FLAGS -Wno-sign-compare";
CC_FLAGS="$CC_FLAGS -Wno-variadic-macros";
CC_LIBRARIES="-lm"
CC="gcc";

if [ "x$1" = "xclean" ]; then
    rm -v lib/*.o build/*;
    exit $?
fi;

if [ "x$1" = "xdebug" ]; then
    CC_FLAGS="-g $CC_FLAGS";
    rm -v lib/*.o build/*;
fi;

objects="";
for file in $(ls lib/*.c); do
    object_path="${file/.c/.o}";
    objects="${objects} ${object_path}";
    if [ -e "$object_path" ]; then
        echo "CC $file already exists";
    else
        echo "CC $file";
        $CC $CC_INCLUDES $CC_FLAGS -c -o "$object_path" "$file" || exit $?;
    fi;
done;

for file in $(ls src/*.c); do
    basename="${file#src/}";
    executable_name="${basename%\.c}"
    if [ -f "./build/$executable_name" ]; then
        echo "CC $file already exists";
    else
        echo "CC $file";
        $CC $CC_INCLUDES $CC_FLAGS -o "./build/$executable_name" \
            $CC_LIBRARIES \
            $objects "$file" || exit $?;
    fi
done
