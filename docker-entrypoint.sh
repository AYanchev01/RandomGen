#!/bin/bash
set -e

run_example() {
    echo "Running RandomGen Example:"
    ./bin/RandomGenExample
}

run_tests() {
    echo "Running RandomGen Tests:"
    ./bin/RandomGenTests
}

case "$1" in
    example)
        run_example
        ;;
    tests)
        run_tests
        ;;
    all)
        run_example
        echo "-----------------------"
        run_tests
        ;;
    *)
        echo "Unknown command: $1"
        echo "Usage: $0 [example|tests|all]"
        exit 1
        ;;
esac