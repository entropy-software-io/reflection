// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

namespace Entropy
{

class TypeInfo;

/// <summary>
/// Smart pointer like object around TypeInfo
/// </summary>
class TypeInfoPtr final
{
public:
    constexpr TypeInfoPtr() noexcept = default;
    inline TypeInfoPtr(const TypeInfo* ptr);
    inline TypeInfoPtr(const TypeInfoPtr& other);
    inline TypeInfoPtr(TypeInfoPtr&& other);
    ~TypeInfoPtr();

    inline const TypeInfo* operator->() const { return _ptr; }

    inline TypeInfoPtr& operator=(const TypeInfoPtr& other);
    inline TypeInfoPtr& operator=(TypeInfoPtr&& other);

    inline operator bool() const { return _ptr != nullptr; }
    inline bool operator==(const TypeInfo* ptr) const { return _ptr == ptr; }
    inline bool operator!=(const TypeInfo* ptr) const { return _ptr != ptr; }

private:
    const TypeInfo* _ptr{};
};

} // namespace Entropy
