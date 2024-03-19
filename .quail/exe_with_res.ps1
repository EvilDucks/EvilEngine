param (
	[string]$project_dir = ""
)

# Because sources are not connected to the executable
#  but placed as a linkfile inside build directory...
#  When running from console we simply need to change the path to build's path.
Set-Location $project_dir
./oneEngine.exe