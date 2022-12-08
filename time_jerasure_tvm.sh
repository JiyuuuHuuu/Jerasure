#!/bin/bash

if [ $1 -eq 0 ]; then
    for i in {1..1000}; do
        ./Examples/jerasure_tvm 10 4 8 128000 123 2 >> jerasure_tvm_result_0.txt
    done
elif [ $1 -eq 1 ]; then
    for i in {1..1000}; do
        ./Examples/time_tvm_decode 10 4 8 128000 123 >> jerasure_tvm_result_1.txt
    done
else
    for i in {1..1000}; do
        ./Examples/time_tvm_multiply 10 4 8 128000 123 >> jerasure_tvm_result_2.txt
    done
fi