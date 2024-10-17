#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <target_name>"
  exit 1
fi

TARGET=$1

echo "FREQ INTEGRATION TESTS"
./$TARGET -S
