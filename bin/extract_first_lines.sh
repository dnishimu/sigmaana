#!/bin/bash

# Output file name
output_file="merged_first_lines.dat"

# Clear the output file if it exists
> $output_file

# Write contents names
header="A0 Z0 Element Nuclide Beam N0_C N1_C N0_E N1_E F5X(mm) F7X(mm) F7_sigma(mm) Sigma_I(mb) Err_Sigma_I(mb) N1_F7PLA_C N1_noF7PLA_C N1_noF7PLAnoF7PPAC_C N1_F7PLA_E N1_noF7PLA_E N1_noF7PLAnoF7PPAC_E"
echo $header > "$output_file"

# Loop through all .dat files in the folder
for file in *.dat; do
  # Skip the output file itself
  if [ "$file" != "$output_file" ]; then
    # Extract the first line and append it to the output file
    head -n 1 "$file" >> "$output_file"
  fi
done

echo "All first lines have been extracted to $output_file"
