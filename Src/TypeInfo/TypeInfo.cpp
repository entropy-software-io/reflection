// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection/TypeInfo/TypeInfo.h"
#include "Entropy/Core/Details/AllocatorTraits.h"

namespace Entropy
{

namespace details
{

TypeInfo* CreateTypeInfo() noexcept { return AllocatorOps::CreateInstance<TypeInfo>(); }

void DestroyTypeInfo(const TypeInfo* typeInfo) noexcept
{
    AllocatorOps::DestroyInstance(const_cast<TypeInfo*>(typeInfo));
}

} // namespace details

//================

TypeInfo::~TypeInfo() { _modules.~ModuleTypes(); }

void TypeInfo::AddRef() const { ++_refCount; }

void TypeInfo::Release() const
{
    int count = --_refCount;
    if (count == 0)
    {
        details::DestroyTypeInfo(this);
    }
}

void TypeInfo::SetTypeName(StringOps::StringType&& name) { _typeName = std::move(name); }

void TypeInfo::SetTypeId(TypeId typeId) { _typeId = typeId; }

bool TypeInfo::CanConstruct() const { return (_constructionFn != nullptr); }

DataObject TypeInfo::Construct() const
{
    if (ENTROPY_LIKELY(CanConstruct()))
    {
        void* data = _constructionFn();
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(this, data, false /* wrapped */, DataObject::DataPointerType::AddressOf);
        }
    }
    return nullptr;
}

bool TypeInfo::CanCopyConstruct() const { return (_copyConstructionFn != nullptr); }

DataObject TypeInfo::DangerousCopyConstruct(const void* src) const
{
    if (ENTROPY_LIKELY(CanCopyConstruct()))
    {
        void* data = _copyConstructionFn(src);
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(this, data, false /* wrapped */, DataObject::DataPointerType::AddressOf);
        }
    }
    return nullptr;
}

bool TypeInfo::CanMoveConstruct() const { return (_moveConstructionFn != nullptr); }

DataObject TypeInfo::DangerousMoveConstruct(void* src) const
{
    if (ENTROPY_LIKELY(CanMoveConstruct()))
    {
        void* data = _moveConstructionFn(src);
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(this, data, false /* wrapped */, DataObject::DataPointerType::AddressOf);
        }
    }
    return nullptr;
}

void TypeInfo::Destruct(void* dataPtr) const
{
    if (ENTROPY_LIKELY(_destructionFn != nullptr))
    {
        _destructionFn(dataPtr);
    }
}

void TypeInfo::SetConstructionHandler(ConstructionHandler&& handler) { _constructionFn = std::move(handler); }

void TypeInfo::SetCopyConstructionHandler(CopyConstructionHandler&& handler)
{
    _copyConstructionFn = std::move(handler);
}

void TypeInfo::SetMoveConstructionHandler(MoveConstructionHandler&& handler)
{
    _moveConstructionFn = std::move(handler);
}

void TypeInfo::SetDestructionHandler(DestructionHandler&& handler) { _destructionFn = std::move(handler); }

bool TypeInfo::IsConst() const { return (_flags & Flags::IsConst) != Flags::None; }

bool TypeInfo::IsPointer() const { return (_flags & Flags::IsPointer) != Flags::None; }

bool TypeInfo::IsArray() const { return (_flags & Flags::IsArray) != Flags::None; }

bool TypeInfo::IsPointerOrArray() const { return (_flags & (Flags::IsPointer | Flags::IsArray)) != Flags::None; }

bool TypeInfo::IsLValueReference() const { return (_flags & Flags::IsLReference) != Flags::None; }

bool TypeInfo::IsRValueReference() const { return (_flags & Flags::IsRReference) != Flags::None; }

bool TypeInfo::IsReference() const { return (_flags & (Flags::IsLReference | Flags::IsRReference)) != Flags::None; }

void TypeInfo::SetIsConst() { _flags |= Flags::IsConst; }

void TypeInfo::SetIsPointer() { _flags |= Flags::IsPointer; }

void TypeInfo::SetIsLReference() { _flags |= Flags::IsLReference; }

void TypeInfo::SetIsRReference() { _flags |= Flags::IsRReference; }

void TypeInfo::SetIsArray() { _flags |= Flags::IsArray; }

bool TypeInfo::IsQualifiedType() const { return _nextUnqualifiedType; }

const TypeInfo* TypeInfo::GetNextUnqualifiedType() const
{
    if (_nextUnqualifiedType)
    {
        return _nextUnqualifiedType;
    }
    return this;
}

const TypeInfo* TypeInfo::GetFullyUnqualifiedType() const
{
    const TypeInfo* ret = this;
    while (ret->IsQualifiedType())
    {
        ret = ret->GetNextUnqualifiedType();
    }
    return ret;
}

void TypeInfo::SetNextUnqualifiedType(const TypeInfo* typeInfo) { _nextUnqualifiedType = typeInfo; }

bool TypeInfo::CanCastTo(const TypeInfo* other) const noexcept
{
    if (ENTROPY_UNLIKELY(other == nullptr))
    {
        return false;
    }

    if (this == other)
    {
        // Same type
        return true;
    }

    // Remove references. When doing the cast in DataObject, we will return a reference anyways.
    if (IsReference() || other->IsReference())
    {
        auto unrefThis  = (IsReference() ? GetNextUnqualifiedType() : this);
        auto unrefOther = (other->IsReference() ? other->GetNextUnqualifiedType() : other);

        return unrefThis->CanCastTo(unrefOther);
    }

    if (IsConst())
    {
        // We don't allow removing our const
        bool canCast = other->IsConst();

        // One caveat is arrays: "const char[]" will be both an array and const, but "const char*" will be just be a
        // pointer (and const at the next level). We need to check for this case explicitly.
        if (IsArray() && other->IsPointer())
        {
            canCast = other->GetNextUnqualifiedType()->IsConst();
        }

        if (!canCast)
        {
            return false;
        }
    }

    // Remove one pointer layer. Arrays can be cast to pointers, but not the other way around.
    if (IsPointerOrArray() || other->IsPointer())
    {
        if (IsPointerOrArray() == other->IsPointer())
        {
            // Can't cast more than one array dimension to pointer
            auto nextType = GetNextUnqualifiedType();
            if (nextType->IsArray())
            {
                return false;
            }

            return nextType->CanCastTo(other->GetNextUnqualifiedType());
        }
        else
        {
            // Mismatch pointer counts
            return false;
        }
    }

    // Compare the base types without any qualifiers we've already checked for
    return (GetFullyUnqualifiedType() == other->GetFullyUnqualifiedType());
}

} // namespace Entropy
