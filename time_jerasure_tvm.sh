#!/bin/bash

for i in {1..1000}; do
    ./Examples/jerasure_tvm 10 4 8 128000 123 2 >> jerasure_tvm_result.txt
done