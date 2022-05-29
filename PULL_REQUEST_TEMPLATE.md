
# PULL REQUEST DESCRIPTION

`ADD CONTENT HERE TO DESCRIBE THE PURPOSE OF THE PULL REQUEST`

# Testing

- [ ] This new/modified code was covered by unit tests. 

- [ ] (insight) Was all tests written using TDD (Test Driven Development) style?

- [ ] The CI (Windows, Linux, OSX) are working without issues. 

- [ ] Was new functionality documented? 

- [ ] The testing steps  1 - 2 below were followed

_step 1_

```bash 
mkdir build; cd build; cmake -DADD_G3LOG_UNIT_TEST=ON ..

// linux/osx alternative, simply run: ./scripts/buildAndRunTests.sh
```

_step 2: use one of these alternatives to run tests:_

- Cross-Platform: `ctest`
- or `ctest -V` for verbose output
- Linux: `make test`
