package rmui

import (
	denv "github.com/jurgen-kluft/ccode/denv"
	cfenc "github.com/jurgen-kluft/cfenc/package"
	rcore "github.com/jurgen-kluft/rcore/package"
)

const (
	repo_path = "github.com\\jurgen-kluft"
	repo_name = "rmui"
)

func GetPackage() *denv.Package {
	name := repo_name

	// dependencies
	corepkg := rcore.GetPackage()
	fencpkg := cfenc.GetPackage()

	// main package
	mainpkg := denv.NewPackage(repo_path, repo_name)
	mainpkg.AddPackage(corepkg)
	mainpkg.AddPackage(fencpkg)

	// main library
	mainlib := denv.SetupCppLibProject(mainpkg, name)
	mainlib.AddDependencies(corepkg.GetMainLib())
	mainlib.AddDependencies(fencpkg.GetMainLib())

	mainpkg.AddMainLib(mainlib)
	return mainpkg
}
