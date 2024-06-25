#!/bin/bash                                                                                                                                      

# Basic setup                                                                                                                                    

# Check if two arguments are provided                                                                                                           \
                                                                                                                                                 
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_52Fe.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "51Fe" "52Fe" "${LABEL}"
run_command "52Fe" "52Fe" "${LABEL}"
run_command "53Fe" "52Fe" "${LABEL}"
run_command "52Co" "52Fe" "${LABEL}"
run_command "53Co" "52Fe" "${LABEL}"
run_command "54Co" "52Fe" "${LABEL}"
run_command "53Ni" "52Fe" "${LABEL}"
run_command "54Ni" "52Fe" "${LABEL}"
run_command "55Ni" "52Fe" "${LABEL}"
run_command "56Ni" "52Fe" "${LABEL}"
run_command "56Cu" "52Fe" "${LABEL}"
run_command "57Cu" "52Fe" "${LABEL}"
run_command "58Cu" "52Fe" "${LABEL}"
