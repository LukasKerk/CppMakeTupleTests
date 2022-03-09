# Comparisons for C++s `std::make_tuple`

`std::make_tuple` is sometimes a bit funky. For example I imagined that the lines
```
return {Example{0,0}, Example{1,1}};
return {{0,0}, {1,1}};
```
would do the same as long as the return value is `std::tuple<Example,Example>`. However, that is not the case. One calls the move and the other one the copy constructor. So I started to dig a bit deeper and test a bit more.

This repository contains different ways to create tuples and tracks how often which constructor (/destructor) is called.
As long as you are on a typical linux machine with `python3` and the python package `pyplotlib`, you can create a visualisation of the results by simply running CMake followed by `make visualize`.
