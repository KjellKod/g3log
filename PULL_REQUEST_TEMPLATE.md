
***PULL REQUEST DESCRIPTION***

`ADD CONTENT HERE TO DESCRIBE THE PURPOSE OF THE PULL REQUEST`

**Testing Advice**
```bash 
mkdir build; cd build; cmake -DADD_G3LOG_UNIT_TEST=ON ..
```
**Run Test Alternatives:** 
- Cross-Platform: `ctest`
- or `ctest -V` for verbose output
- Linux: `make test`
- Linux/OSX: `./scripts/buildAndRunTests.sh`

***CHECKLIST TO COMPLETE***
- [ ] **TDD**: New/modified code must be backed down with unit test - preferably _Test Driven Development_ style development)
- [ ] **Documentation**:  All new/modified functionality should be backed up with API documentation (API.markdown or README.markdown)


**Cross-Platform Testing**
- [ ] CI GitActions:  (Linux, OSX) + AppVeyor-CI (Windows)\
- [ ] Local/VM testing, at least one of the following: Windows, Linux, OSX
