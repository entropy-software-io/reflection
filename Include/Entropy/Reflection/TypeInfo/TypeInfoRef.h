// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

namespace Entropy
{

class TypeInfo;

/// <summary>
/// Holds a reference to a type info object. This is rarely needed, but is useful when you want to ensure a type info
/// method is available past when the type info would have normally been destroyed on app exit.
/// </summary>
class TypeInfoRef final
{
public:
    constexpr TypeInfoRef() noexcept = default;
    TypeInfoRef(const TypeInfo* ptr);
    TypeInfoRef(const TypeInfoRef& other);
    TypeInfoRef(TypeInfoRef&& other);
    ~TypeInfoRef();

    inline operator const TypeInfo*() const { return _ptr; }
    inline const TypeInfo* operator->() const { return _ptr; }

    TypeInfoRef& operator=(const TypeInfoRef& other);
    TypeInfoRef& operator=(TypeInfoRef&& other);

    inline operator bool() const { return _ptr != nullptr; }
    inline bool operator==(const TypeInfo* ptr) const { return _ptr == ptr; }
    inline bool operator!=(const TypeInfo* ptr) const { return _ptr != ptr; }

private:
    const TypeInfo* _ptr{};
};

} // namespace Entropy
