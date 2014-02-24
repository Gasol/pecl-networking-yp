#!/bin/sh
# Sould source this file after phpize (Copied Makefile.global)
# See https://bugs.php.net/bug.php?id=60285

realpath() {
	file=$1
	cd $(dirname $file)
	file=$(basename $file)
	if [ -h "$file" ]; then
		echo $file
		readlink $(readlink $file)
	else
		dir=$(pwd -P)
		echo "$dir/$file"
	fi
}

BASEDIR=$(dirname $(dirname $(realpath $0)))
MAKEFILE="$BASEDIR/Makefile.global"

if ! grep -q 'TEST_RESULT_EXIT_CODE' $MAKEFILE; then
	echo "Applying patch to fix PHP Bug #60285"
	sed -i -re 's|-(@if test ! -z "\$\(PHP_EXECUTABLE\)")|\1|' $MAKEFILE
	sed -i -re '/TESTS/a \\t\tTEST_RESULT_EXIT_CODE=$$?; \\' $MAKEFILE
	sed -i -re '/rm \$\(top_builddir\)\/tmp-php\.ini; \\/a \\t\texit $$TEST_RESULT_EXIT_CODE; \\' $MAKEFILE
fi
