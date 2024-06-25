#!/bin/bash

# Basic setup
# Check if two arguments are provided                                                                        
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 NUCLIDE BEAM LABEL"
    echo "Example ./bin/Batch_Sigma_calc.sh \"42Ca\" \"41Ca\" \"1\" "
    echo "You can use any LABEL as you like" 
    exit 1
fi

# Basic setup                                                                                                 
NUCLIDE="$1"
BEAM="$2"
LABEL="$3"

MASS=$(echo $NUCLIDE | grep -o -E '[0-9]+')
SYMBOL=$(echo $NUCLIDE | grep -o -E '[a-zA-Z]+')

# Define the atomic numbers for different elements (add more as needed)
declare -A ATOMIC_NUMBERS
ATOMIC_NUMBERS=( ["H"]=1 ["He"]=2 ["Li"]=3 ["Be"]=4 ["B"]=5 ["C"]=6 ["N"]=7 ["O"]=8 ["F"]=9 ["Ne"]=10
                 ["Na"]=11 ["Mg"]=12 ["Al"]=13 ["Si"]=14 ["P"]=15 ["S"]=16 ["Cl"]=17 ["Ar"]=18
                 ["K"]=19 ["Ca"]=20 ["Sc"]=21 ["Ti"]=22 ["V"]=23 ["Cr"]=24 ["Mn"]=25 ["Fe"]=26
                 ["Co"]=27 ["Ni"]=28 ["Cu"]=29 ["Zn"]=30 ["Ga"]=31 ["Ge"]=32 ["As"]=33 ["Se"]=34
                 ["Br"]=35 ["Kr"]=36 ["Rb"]=37 ["Sr"]=38 ["Y"]=39 ["Zr"]=40 ["Nb"]=41 ["Mo"]=42
                 ["Tc"]=43 ["Ru"]=44 ["Rh"]=45 ["Pd"]=46 ["Ag"]=47 ["Cd"]=48 ["In"]=49 ["Sn"]=50
                 ["Sb"]=51 ["Te"]=52 ["I"]=53 ["Xe"]=54 ["Cs"]=55 ["Ba"]=56 ["La"]=57 ["Ce"]=58
                 ["Pr"]=59 ["Nd"]=60 ["Pm"]=61 ["Sm"]=62 ["Eu"]=63 ["Gd"]=64 ["Tb"]=65 ["Dy"]=66
                 ["Ho"]=67 ["Er"]=68 ["Tm"]=69 ["Yb"]=70 ["Lu"]=71 ["Hf"]=72 ["Ta"]=73 ["W"]=74
                 ["Re"]=75 ["Os"]=76 ["Ir"]=77 ["Pt"]=78 ["Au"]=79 ["Hg"]=80 ["Tl"]=81 ["Pb"]=82
                 ["Bi"]=83 ["Po"]=84 ["At"]=85 ["Rn"]=86 ["Fr"]=87 ["Ra"]=88 ["Ac"]=89 ["Th"]=90
                 ["Pa"]=91 ["U"]=92 ["Np"]=93 ["Pu"]=94 ["Am"]=95 ["Cm"]=96 ["Bk"]=97 ["Cf"]=98
                 ["Es"]=99 ["Fm"]=100 ["Md"]=101 ["No"]=102 ["Lr"]=103 ["Rf"]=104 ["Db"]=105 ["Sg"]=106
                 ["Bh"]=107 ["Hs"]=108 ["Mt"]=109 ["Ds"]=110 ["Rg"]=111 ["Cn"]=112 ["Nh"]=113 ["Fl"]=114
                 ["Mc"]=115 ["Lv"]=116 ["Ts"]=117 ["Og"]=118 )

# Get the atomic number for the element symbol
Z0=${ATOMIC_NUMBERS[$SYMBOL]}

# Copy the default input file to the specific input file
cp ./input/input_default.dat "./input/input_${NUCLIDE}_${BEAM}_C_${LABEL}.dat"

# Function definition
function update_parameter {
    local param=$1
    local value=$2
    local filename=$3
    awk -v param="$param" -v value="$value" '$1 == param {$2 = value} 1' $filename > tmp && mv tmp $filename
}

# Initial execution
BASE_FILENAME="input_${NUCLIDE}_${BEAM}_C_${LABEL}"
BASE_ROOT_FILE="treefiles/merge_${BEAM}_C.root"

# Update A0 and Z0 in the copied file
update_parameter "A0" $MASS "./input/${BASE_FILENAME}.dat"
update_parameter "Z0" ${ATOMIC_NUMBERS[$SYMBOL]} "./input/${BASE_FILENAME}.dat"
update_parameter "R_n" "0.1" "./input/${BASE_FILENAME}.dat"
update_parameter "Flag_All" "0" "./input/${BASE_FILENAME}.dat"
update_parameter "Flag_Emittance" "0" "./input/${BASE_FILENAME}.dat"
update_parameter "Flag_Fix_up_para" "0" "./input/${BASE_FILENAME}.dat"
echo "y" | root -l ${BASE_ROOT_FILE} -q "macros/Sigma_calc.C(\"${BASE_FILENAME}.dat\")"

# Update parameters and re-execute
update_parameter "R_n" "1." "./input/${BASE_FILENAME}_rev.dat"
update_parameter "Flag_All" "0" "./input/${BASE_FILENAME}_rev.dat"
update_parameter "Flag_Emittance" "0" "./input/${BASE_FILENAME}_rev.dat"
update_parameter "Flag_Fix_up_para" "0" "./input/${BASE_FILENAME}_rev.dat"
echo "y" | root -l ${BASE_ROOT_FILE} -q "macros/Sigma_calc.C(\"${BASE_FILENAME}_rev.dat\")"

update_parameter "R_n" "1." "./input/${BASE_FILENAME}_rev_rev.dat"
update_parameter "Flag_All" "1" "./input/${BASE_FILENAME}_rev_rev.dat"
update_parameter "Flag_Emittance" "1" "./input/${BASE_FILENAME}_rev_rev.dat"
update_parameter "Flag_Fix_up_para" "1" "./input/${BASE_FILENAME}_rev_rev.dat"
echo "y" | root -l ${BASE_ROOT_FILE} -q "macros/Sigma_calc.C(\"${BASE_FILENAME}_rev_rev.dat\")"

cp "./input/${BASE_FILENAME}_rev_rev.dat" "./input/input_${NUCLIDE}_${BEAM}_E_${LABEL}.dat"

# Perform operations on the next file set
BASE_FILENAME="input_${NUCLIDE}_${BEAM}_E_${LABEL}"
BASE_ROOT_FILE="treefiles/merge_${BEAM}_E.root"

update_parameter "R_n" "0.1" "./input/${BASE_FILENAME}.dat"
update_parameter "Flag_All" "0" "./input/${BASE_FILENAME}.dat"
update_parameter "Flag_Emittance" "0" "./input/${BASE_FILENAME}.dat"
update_parameter "Flag_Fix_up_para" "1" "./input/${BASE_FILENAME}.dat"
echo "y" | root -l ${BASE_ROOT_FILE} -q "macros/Sigma_calc.C(\"${BASE_FILENAME}.dat\")"

update_parameter "R_n" "1." "./input/${BASE_FILENAME}_rev.dat"
update_parameter "Flag_All" "0" "./input/${BASE_FILENAME}_rev.dat"
update_parameter "Flag_Emittance" "0" "./input/${BASE_FILENAME}_rev.dat"
update_parameter "Flag_Fix_up_para" "1" "./input/${BASE_FILENAME}_rev.dat"
echo "y" | root -l ${BASE_ROOT_FILE} -q "macros/Sigma_calc.C(\"${BASE_FILENAME}_rev.dat\")"

update_parameter "R_n" "1." "./input/${BASE_FILENAME}_rev_rev.dat"
update_parameter "Flag_All" "1" "./input/${BASE_FILENAME}_rev_rev.dat"
update_parameter "Flag_Emittance" "1" "./input/${BASE_FILENAME}_rev_rev.dat"
update_parameter "Flag_Fix_up_para" "1" "./input/${BASE_FILENAME}_rev_rev.dat"
echo "y" | root -l ${BASE_ROOT_FILE} -q "macros/Sigma_calc.C(\"${BASE_FILENAME}_rev_rev.dat\")"

RESULTS_SUMMARY="./results_${LABEL}"
if [ ! -d "$RESULTS_SUMMARY" ]; then
  mkdir -p "$RESULTS_SUMMARY"
  cp extract_first_lines.sh "$RESULTS_SUMMARY/"
fi
root -l -b -q "macros/results.C(\"${NUCLIDE}\",\"${BEAM}\",\"${LABEL}\")"
