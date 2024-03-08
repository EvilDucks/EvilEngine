param (
	[string]$project_build_dir = ""
)

# For each profile in catalog.
Remove-Item -Path $project_build_dir -Recurse -Include CMakeCache.txt
