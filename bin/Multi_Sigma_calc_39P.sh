#!/bin/bash                                                                                                                                                         

# Basic setup                                                                                                                                                       

# Check if two arguments are provided                                                                                                                               

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_39P.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "37P" "39P" "${LABEL}"
run_command "38P" "39P" "${LABEL}"
run_command "39P" "39P" "${LABEL}"
run_command "40P" "39P" "${LABEL}"
run_command "40S" "39P" "${LABEL}"
run_command "41S" "39P" "${LABEL}"
run_command "42S" "39P" "${LABEL}"
run_command "43S" "39P" "${LABEL}"
run_command "43Cl" "39P" "${LABEL}"
run_command "44Cl" "39P" "${LABEL}"
run_command "45Cl" "39P" "${LABEL}"
run_command "45Ar" "39P" "${LABEL}"
run_command "46Ar" "39P" "${LABEL}"
run_command "35Si" "39P" "${LABEL}"
run_command "36Si" "39P" "${LABEL}"
run_command "37Si" "39P" "${LABEL}"
run_command "38Si" "39P" "${LABEL}"
run_command "33Al" "39P" "${LABEL}"
run_command "34Al" "39P" "${LABEL}"
run_command "35Al" "39P" "${LABEL}"
run_command "36Al" "39P" "${LABEL}"
