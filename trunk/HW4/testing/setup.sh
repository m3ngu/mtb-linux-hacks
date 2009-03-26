#!/bin/bash
useradd alice
useradd bob
useradd carol
./change-weight alice 15
./change-weight bob 10
./change-weight carol 5
./change-weight root 10