# DRAFT (work in progress)
Multiple label propagation on graphs with GraphSage. Classical algorithms, like Label Propagation, solve this task in a deterministic way. This is a probabilistic aproach using GraphSage algorithm with neighbor label counts as node features.

## Install
Need to install TBB (Threading Building Blocks) library to use `parallel_for` with job stealing and `std::execution` parallel policy (`clang` does not support the policy yet).

###### Apple:
`brew install tbb`

###### Debian:  
`sudo apt install libtbb-dev`

###### CentOS:  
`yum install tbb-devel`  

##### Build:  

`mkdir build && cd build && cmake .. && cmake --build . && cd ..`

##### Test:

`./build/test`