// A Python extension module written in C++: Python hands Arrow arrays to C++,
// C++ computes with sparrow-compute and hands the result back.
// sparrow-rockfinch does the Arrow PyCapsule Interface plumbing.

#include <format>
#include <string>
#include <type_traits>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <sparrow.hpp>
#include <sparrow/config/sparrow_version.hpp>
#include <sparrow-compute.hpp>
#include <sparrow-rockfinch/config/sparrow_rockfinch_version.hpp>
#include <sparrow-rockfinch/sparrow_array_python_class.hpp>

namespace nb = nanobind;
namespace sp = sparrow;
using sp::rockfinch::SparrowArray;
using namespace nb::literals;

namespace
{
    // Imports any object implementing __arrow_c_array__ (pyarrow.Array, SparrowArray, ...):
    // the sparrow array takes over the ArrowArray and keeps the producer's buffers alive
    SparrowArray import_array(nb::handle data)
    {
        nb::tuple capsules = nb::cast<nb::tuple>(data.attr("__arrow_c_array__")());
        return SparrowArray(capsules[0].ptr(), capsules[1].ptr());
    }

    const sp::primitive_array<double>& as_float64(const SparrowArray& data)
    {
        const sp::primitive_array<double>* typed = nullptr;
        data.get_array().visit(
            [&typed]<class T>(const T& array)
            {
                if constexpr (std::is_same_v<T, sp::primitive_array<double>>)
                {
                    typed = &array;
                }
            }
        );
        if (typed == nullptr || data.get_array().null_count() != 0)
        {
            throw nb::type_error("expected a float64 array without nulls");
        }
        return *typed;
    }
}

NB_MODULE(rockfinch_demo, m)
{
    m.attr("sparrow_version") = std::format(
        "{}.{}.{}",
        sp::SPARROW_VERSION_MAJOR,
        sp::SPARROW_VERSION_MINOR,
        sp::SPARROW_VERSION_PATCH
    );
    m.attr("rockfinch_version") = std::format(
        "{}.{}.{}",
        sp::rockfinch::SPARROW_ROCKFINCH_VERSION_MAJOR,
        sp::rockfinch::SPARROW_ROCKFINCH_VERSION_MINOR,
        sp::rockfinch::SPARROW_ROCKFINCH_VERSION_PATCH
    );

    nb::class_<SparrowArray>(m, "SparrowArray", "A sparrow array exposed through the Arrow PyCapsule Interface")
        .def(
            "__init__",
            [](SparrowArray* self, nb::handle data)
            {
                new (self) SparrowArray(import_array(data));
            },
            "data"_a
        )
        .def(
            "__arrow_c_array__",
            [](const SparrowArray& self, nb::handle /*requested_schema*/)
            {
                auto [schema, array] = self.export_to_capsules();
                return nb::make_tuple(nb::steal(schema), nb::steal(array));
            },
            "requested_schema"_a = nb::none()
        )
        .def("__len__", &SparrowArray::size)
        .def(
            "__repr__",
            [](const SparrowArray& self)
            {
                return std::format("{}", self.get_array());
            }
        );

    m.def(
        "make_array",
        []
        {
            return SparrowArray(sp::array(sp::primitive_array<double>{3.0, 5.0, 8.0, 20.0}));
        },
        "A float64 array built in C++"
    );

    m.def(
        "hypot",
        [](nb::handle x, nb::handle y)
        {
            const SparrowArray x_array = import_array(x);
            const SparrowArray y_array = import_array(y);
            const auto& a = as_float64(x_array);
            const auto& b = as_float64(y_array);
            if (a.size() != b.size())
            {
                throw nb::value_error("x and y must have the same length");
            }
            // sparrow-compute reads the Python-owned buffers in place
            auto result = sp::compute::eval(sp::compute::sqrt(a * a + b * b));
            return SparrowArray(sp::array(std::move(result)));
        },
        "x"_a,
        "y"_a,
        "Element-wise sqrt(x**2 + y**2) of two float64 Arrow arrays, computed in C++ with sparrow-compute"
    );
}
