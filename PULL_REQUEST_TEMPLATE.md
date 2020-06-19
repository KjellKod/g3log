- [ ] **TDD** 

New/modified code must be backed down with unit test - preferably TDD style development)


- [ ] **Documentation**

All new/modified functionality should be backed up with API documentation (API.markdown or README.markdown)


**Cross-Platform Testing**
- [ ] Travis-CI (Linux, OSX) + AppVeyor-CI (Windows)\
- [ ] *Optional:* Local/VM testing: Windows
- [ ] *Optional:* Local/VM testing: OSX
- [ ] *Optional:* Local/VM testing: Linux


**Testing Advice**
```bash 
mkdir build; cd build; cmake -DADD_G3LOG_UNIT_TEST=ON ..
```

**Run Test Alternatives:** 
- Cross-Platform: `ctest`
- or `ctest -V` for verbose output
- Linux: `make test`
