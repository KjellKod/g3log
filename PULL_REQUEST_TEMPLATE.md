- [ ] Follow TDD practice. (new/modified code must be backed down with unit test - preferably TDD style development)
- [ ] All new/modified functionality should be backed up with API documentation (API.markdown or README.markdown)
- [ ] Cross Platform validation with CI: Windows, Ubuntu, OSX support must work 
- [ ] *Optional but recommended*: Cross Platfrom validation with local or VM setup. 


**Testing Advice**
```bash 
mkdir build; cd build; cmake -DADD_G3LOG_UNIT_TEST=ON ..
```

**Run Test Alternatives:** 
- Cross-Platform: `ctest`
- or `ctest -V` for verbose output
- Linux: `make test`
