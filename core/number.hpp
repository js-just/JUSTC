#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <boost/multiprecision/cpp_dec_float.hpp>

using BigNum =
    boost::multiprecision::number<
        boost::multiprecision::cpp_dec_float<50>
    >;
using LargeNum =
    boost::multiprecision::number<
        boost::multiprecision::cpp_dec_float<100>
    >;
using HugeNum =
    boost::multiprecision::number<
        boost::multiprecision::cpp_dec_float<1000>
    >;
using GiantNum =
    boost::multiprecision::number<
        boost::multiprecision::cpp_dec_float<10000>
    >;
using ColossalNum =
    boost::multiprecision::number<
        boost::multiprecision::cpp_dec_float<100000>
    >;


using JUSTCnum = std::variant<
    double,
    BigNum,
    LargeNum,
    HugeNum,
    GiantNum,
    ColossalNum
>;

#endif
