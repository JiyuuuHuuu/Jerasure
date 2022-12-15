#!/bin/bash

while getopts p:d:n:m: flag
do
    case "${flag}" in
        p) parity=${OPTARG};;
        d) data=${OPTARG};;
        n) num_n=${OPTARG};;
        m) mode=${OPTARG};;
    esac
done

if [ $mode -eq 0 ]; then
    for i in {1..1000}; do
        ./Examples/jerasure_tvm ${data} ${parity} 8 ${num_n} 123 2 >> jerasure_tvm_result_${data}_${parity}_${num_n}_m${mode}.txt
    done
elif [ $mode -eq 2 ]; then
    for i in {1..1000}; do
        ./Examples/time_tvm_multiply ${data} ${parity} 8 ${num_n} 123 0 >> jerasure_tvm_result_${data}_${parity}_${num_n}_m${mode}.txt
    done
elif [ $mode -eq 3 ]; then
    for i in {1..1000}; do
        ./Examples/time_tvm_multiply ${data} ${parity} 8 ${num_n} 123 1 >> jerasure_tvm_result_${data}_${parity}_${num_n}_m${mode}.txt
    done
fi