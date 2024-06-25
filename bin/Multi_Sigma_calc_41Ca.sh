#!/bin/bash                                                                                                                                                         

# Basic setup                                                                                                                                                       
# Check if two arguments are provided                                                                                                                               
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_41Ca.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "44Ti" "41Ca" "${LABEL}"
run_command "45Ti" "41Ca" "${LABEL}"
run_command "42Sc" "41Ca" "${LABEL}"
run_command "43Sc" "41Ca" "${LABEL}"
run_command "44Sc" "41Ca" "${LABEL}"
run_command "40Ca" "41Ca" "${LABEL}"
run_command "41Ca" "41Ca" "${LABEL}"
run_command "42Ca" "41Ca" "${LABEL}"
run_command "39K" "41Ca" "${LABEL}"
run_command "40K" "41Ca" "${LABEL}"
run_command "41K" "41Ca" "${LABEL}"
run_command "37Ar" "41Ca" "${LABEL}"
run_command "38Ar" "41Ca" "${LABEL}"
run_command "35Cl" "41Ca" "${LABEL}"
run_command "36Cl" "41Ca" "${LABEL}"
