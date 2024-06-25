#!/bin/bash                                                                                                   

# Basic setup                                                                                                \
                                                                                                              
# Check if two arguments are provided                                                                        \
                                                                                                              
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_34P.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "33P" "34P" "${LABEL}"
run_command "34P" "34P" "${LABEL}"
run_command "35P" "34P" "${LABEL}"
run_command "35S" "34P" "${LABEL}"
run_command "36S" "34P" "${LABEL}"
run_command "37S" "34P" "${LABEL}"
run_command "38Cl" "34P" "${LABEL}"
run_command "39Cl" "34P" "${LABEL}"
run_command "40Ar" "34P" "${LABEL}"
run_command "41Ar" "34P" "${LABEL}"
run_command "31Si" "34P" "${LABEL}"
run_command "32Si" "34P" "${LABEL}"
run_command "30Al" "34P" "${LABEL}"
