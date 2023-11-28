# Codespaces 

You can experiment with codespaces and g3log. 

## Learn about Github Codespaces 
For an introduction to codespaces you can check out [example c++ codespace](https://github.com/microsoft/vscode-remote-try-cpp/tree/main) and [using-github-codespaces-with-github-cli](https://docs.github.com/en/codespaces/developing-in-a-codespace/using-github-codespaces-with-github-cli)


# Commandline codespaces Quick Reference 

1. List all your codespaces `gh codespace list`
2.  Create a new codespace `gh codespace create -r OWNER/REPO_NAME [-b BRANCH]`. Ref [docs/github: Creating a codespace for a repository](https://docs.github.com/en/codespaces/developing-in-a-codespace/creating-a-codespace-for-a-repository)
3. View codebase details `gh codespace view`
4. Stop `gh codespace stop -c CODESPACE-NAME`
5. Delete `gh codespace delete -c CODESPACE-NAME`
6. Rebuild `gh codespace rebuild`
7. Rename `gh codespace edit -c CODESPACE-NAME -d DISPLAY-NAME`
8. SSH into REMOTE codespace `gh codespace ssh -c CODESPACE-NAME`
9. Open a remote codespace in CVisual Studio `gh codespace code -c CODESPACE-NAME` (ref: [github:doc cs studio](https://docs.github.com/en/codespaces/developing-in-a-codespace/using-github-codespaces-in-visual-studio-code))
10. Copy local file to/from codespace `gh codespace cp [-r] SOURCE(S) DESTINATION`. Example: Copy a file from the local machine to the $HOME directory of a codespace: `gh codespace cp myfile.txt remote:`. Example Copy a file from a codespace to the current directory on the local machine: `gh codespace cp remote:myfile.txt .` (more information available [here](https://cli.github.com/manual/gh_codespace_cp))


# Try g3log in a local dev container. 

Please note that this will build g3log as if it's on a Debian Linux platform.

1. Clone this repository to your local filesystem.
2. Start Visual Studio Code. Press F1 and select the `Dev Containers: Open Folder in Container...` command.
3. Select the cloned copy of this g3log folder, wait for the container to start, and try things out! You should have debian C++ environment at hand. 

### Example cmake configuration and build
```
Open a terminal in Visual Studio Code
mkdir debianbuild
cd debianbuild
cmake -DADD_G3LOG_UNIT_TEST=ON -DADD_G3LOG_BENCH_PERFORMANCE=ON ..
make -j 
```

### Example runs
1. performance test in the container `./g3log-performance-threaded_mean 4`
2. unit tests `ctest -v`
3. Try a fatal example with dumped stack trace `./g3log-FATAL-contract`