set +e >> /dev/null
set +o >> /dev/null

STD_OUT=`mktemp`
STD_ERR=`mktemp`

# Clean up any left overs from last time
rm -f $STD_OUT $STD_ERR
touch $STD_OUT
touch $STD_ERR

usage () {
    echo "usage: $0 options

    This script will pull the latest version of the project
    this script resides in and build it

    OPTIONS:
      -h  Show this message
      -p  Only build if new content was pulled
      -s  Show only minimal output"
}

while getopts "hps" OPTION
do
    case $OPTION in
    h)
        usage
        exit 0
        ;;
    p)
        EARLY_QUIT=1
        ;;
    s)
        SILENT=1
        ;;
    esac
done

CURRENT_PROJECT=${PWD##*/}
echo "Building $CURRENT_PROJECT ..."

execute_silently () {
    if ! "$@" < /dev/null >$STD_OUT 2>$STD_ERR; then
        cat $STD_ERR >&2
        rm -f $STD_OUT $STD_ERR
        echo -e "\e[1;31mFailed.\e[0m"
        exit 1
    fi
}

execute () {
    echo "Executing command $@"
    if [ ! -z "$SILENT" ]
        then
        execute_silently "$@"
    elif ! "$@" | tee $STD_OUT 2 | tee $STD_ERR; then
        echo -e "\e[1;31mFailed.\e[0m"
        exit 1
    fi
    echo -e "\e[1;32mSucceeded.\e[0m"
    if [ "$1" != "git" ]
        then
        rm -f $STD_OUT $STD_ERR
    fi
}

git_pull () {
    execute git pull
    OUTPUT=`tail -n1 $STD_OUT`
    rm -f $STD_OUT $STD_ERR
    NO_CHANGES='Already up-to-date.'
    if [ "$OUTPUT" == "$NO_CHANGES" ]
        then
        echo -e "\e[1;32mNo new changes.\e[0m"
        if [ ! -z "$EARLY_QUIT" ]
            then
            exit 0
        fi
    else
        echo -e "\e[1;32mNew changes found.\e[0m"
    fi
}

git_pull
execute ./autogen.sh
execute ./configure
execute make clean
execute make check
execute sudo make install
execute sudo ldconfig
