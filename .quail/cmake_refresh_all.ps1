param (
	[string]$project_dir = ""
)

$ninja_path = "-DCMAKE_MAKE_PROGRAM=D://ProgramFiles//ninja.exe"
$crc  	    = "-DCMAKE_RC_COMPILER='D://ProgramFiles//LLVM//bin//llvm-rc.exe'"
$cc   	    = "-DCMAKE_C_COMPILER='D://ProgramFiles//LLVM//bin//clang.exe'"
$ccp  	    = "-DCMAKE_CXX_COMPILER='D://ProgramFiles//LLVM//bin//clang++.exe'"
$type_deb   = "-DCMAKE_BUILD_TYPE=Debug"
$type_rel   = "-DCMAKE_BUILD_TYPE=Release"

# For each profile!
cmake -DBUILD_TYPE=1 $crc $cc $ccp $type_deb $ninja_path -G Ninja -S $project_dir -B $project_dir/build/win_debug
cmake -DBUILD_TYPE=1 $crc $cc $ccp $type_rel $ninja_path -G Ninja -S $project_dir -B $project_dir/build/win_release
cmake -DBUILD_TYPE=0 $crc $cc $ccp $type_deb $ninja_path -G Ninja -S $project_dir -B $project_dir/build/agn_debug
cmake -DBUILD_TYPE=0 $crc $cc $ccp $type_rel $ninja_path -G Ninja -S $project_dir -B $project_dir/build/agn_release
