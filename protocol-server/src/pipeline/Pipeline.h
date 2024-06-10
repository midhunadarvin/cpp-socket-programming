#pragma once

template <typename T>
using PipelineFunction = void *(*)(T *ptr, void *lptr);
