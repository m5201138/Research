file(REMOVE_RECURSE
  "bin/TwString.pdb"
  "bin/TwString"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/TwString.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
