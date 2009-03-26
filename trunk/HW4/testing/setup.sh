#!/bin/bash
useradd alice
useradd bob
useradd carol
./change-weight alice 20
./change-weight bob 10
./change-weight carol 10
./change-weight root 10

