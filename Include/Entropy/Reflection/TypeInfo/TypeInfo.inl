// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

namespace Entropy
{

namespace details
{

inline TypeInfo* CreateTypeInfo() noexcept
{
    using ContainerTraits = ReflectionContainerTraits<TypeInfo>;

    ContainerTraits::Allocator<TypeInfo> alloc;
    TypeInfo* typeInfo = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
    if (ENTROPY_LIKELY(typeInfo))
    {
        std::allocator_traits<decltype(alloc)>::construct(alloc, typeInfo);
    }

    return typeInfo;
}

inline void DestroyTypeInfo(const TypeInfo* typeInfo) noexcept
{
    using ContainerTraits = ReflectionContainerTraits<TypeInfo>;

    if (ENTROPY_LIKELY(typeInfo))
    {
        ContainerTraits::Allocator<TypeInfo> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, typeInfo);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, const_cast<TypeInfo*>(typeInfo), 1);
    }
}

} // namespace details

//================

inline TypeInfo::~TypeInfo() { _modules.~ModuleTypes(); }

inline void TypeInfo::AddRef() const { ++_refCount; }

inline void TypeInfo::Release() const
{
    int count = --_refCount;
    if (count == 0)
    {
        details::DestroyTypeInfo(this);
    }
}

inline void TypeInfo::SetTypeName(ContainerTraits::StringType&& name) { _typeName = std::move(name); }

inline void TypeInfo::SetTypeId(TypeId typeId) { _typeId = typeId; }

inline bool TypeInfo::CanConstruct() const { return (_constructionFn != nullptr); }

inline DataObject TypeInfo::Construct() const
{
    if (ENTROPY_LIKELY(CanConstruct()))
    {
        void* data = _constructionFn();
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(this, data, true /* deallocate */);
        }
    }
    return nullptr;
}

inline bool TypeInfo::CanCopyConstruct() const { return (_copyConstructionFn != nullptr); }

inline DataObject TypeInfo::DangerousCopyConstruct(const void* src) const
{
    if (ENTROPY_LIKELY(CanCopyConstruct()))
    {
        void* data = _copyConstructionFn(src);
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(this, data, true /* deallocate */);
        }
    }
    return nullptr;
}

inline bool TypeInfo::CanMoveConstruct() const { return (_moveConstructionFn != nullptr); }

inline DataObject TypeInfo::DangerousMoveConstruct(void* src) const
{
    if (ENTROPY_LIKELY(CanMoveConstruct()))
    {
        void* data = _moveConstructionFn(src);
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(this, data, true /* deallocate */);
        }
    }
    return nullptr;
}

inline void TypeInfo::Destruct(void* dataPtr) const
{
    if (ENTROPY_LIKELY(_destructionFn != nullptr))
    {
        _destructionFn(dataPtr);
    }
}

inline void TypeInfo::SetConstructionHandler(ConstructionHandler&& handler) { _constructionFn = std::move(handler); }

inline void TypeInfo::SetCopyConstructionHandler(CopyConstructionHandler&& handler)
{
    _copyConstructionFn = std::move(handler);
}

inline void TypeInfo::SetMoveConstructionHandler(MoveConstructionHandler&& handler)
{
    _moveConstructionFn = std::move(handler);
}

inline void TypeInfo::SetDestructionHandler(DestructionHandler&& handler) { _destructionFn = std::move(handler); }

inline bool TypeInfo::IsConst() const { return (_flags & Flags::IsConst) != Flags::None; }

inline bool TypeInfo::IsPointer() const { return (_flags & Flags::IsPointer) != Flags::None; }

inline bool TypeInfo::IsArray() const { return (_flags & Flags::IsArray) != Flags::None; }

inline bool TypeInfo::IsPointerOrArray() const { return (_flags & (Flags::IsPointer | Flags::IsArray)) != Flags::None; }

inline bool TypeInfo::IsLValueReference() const { return (_flags & Flags::IsLReference) != Flags::None; }

inline bool TypeInfo::IsRValueReference() const { return (_flags & Flags::IsRReference) != Flags::None; }

inline bool TypeInfo::IsReference() const
{
    return (_flags & (Flags::IsLReference | Flags::IsRReference)) != Flags::None;
}

inline void TypeInfo::SetIsConst() { _flags |= Flags::IsConst; }

inline void TypeInfo::SetIsPointer() { _flags |= Flags::IsPointer; }

inline void TypeInfo::SetIsLReference() { _flags |= Flags::IsLReference; }

inline void TypeInfo::SetIsRReference() { _flags |= Flags::IsRReference; }

inline void TypeInfo::SetIsArray() { _flags |= Flags::IsArray; }

inline bool TypeInfo::IsQualifiedType() const { return _nextUnqualifiedType; }

inline const TypeInfo* TypeInfo::GetNextUnqualifiedType() const
{
    if (_nextUnqualifiedType)
    {
        return _nextUnqualifiedType;
    }
    return this;
}

inline const TypeInfo* TypeInfo::GetFullyUnqualifiedType() const
{
    const TypeInfo* ret = this;
    while (ret->IsQualifiedType())
    {
        ret = ret->GetNextUnqualifiedType();
    }
    return ret;
}

inline void TypeInfo::SetNextUnqualifiedType(const TypeInfo* typeInfo) { _nextUnqualifiedType = typeInfo; }

inline bool TypeInfo::CanCastTo(const TypeInfo* other) const noexcept
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
