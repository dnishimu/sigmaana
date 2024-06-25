#!/bin/bash

# Basic setup                                                                                                                             
# Check if two arguments are provided                                                                                                     
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_31P.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "33S" "31P" "${LABEL}"
run_command "31P" "31P" "${LABEL}"
run_command "32P" "31P" "${LABEL}"
run_command "33P" "31P" "${LABEL}"
run_command "33S" "31P" "${LABEL}"
run_command "34S" "31P" "${LABEL}"
run_command "29Si" "31P" "${LABEL}"
run_command "30Si" "31P" "${LABEL}"
run_command "31Si" "31P" "${LABEL}"
run_command "28Al" "31P" "${LABEL}"
run_command "29Al" "31P" "${LABEL}"
