// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

namespace Entropy
{

class TypeInfo;

/// <summary>
/// Holds a reference to a type info object. It is recommended that you use this when you need a TypeInfo class member
/// variable.
/// </summary>
class TypeInfoRef final
{
public:
    constexpr TypeInfoRef() noexcept = default;
    inline TypeInfoRef(const TypeInfo* ptr);
    inline TypeInfoRef(const TypeInfoRef& other);
    inline TypeInfoRef(TypeInfoRef&& other);
    ~TypeInfoRef();

    inline operator const TypeInfo*() const { return _ptr; }
    inline const TypeInfo* operator->() const { return _ptr; }

    inline TypeInfoRef& operator=(const TypeInfoRef& other);
    inline TypeInfoRef& operator=(TypeInfoRef&& other);

    inline operator bool() const { return _ptr != nullptr; }
    inline bool operator==(const TypeInfo* ptr) const { return _ptr == ptr; }
    inline bool operator!=(const TypeInfo* ptr) const { return _ptr != ptr; }

private:
    const TypeInfo* _ptr{};
};

} // namespace Entropy
