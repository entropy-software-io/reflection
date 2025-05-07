// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

namespace Entropy
{

namespace details
{

TypeInfo* CreateTypeInfo() noexcept
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

void DestroyTypeInfo(const TypeInfo* typeInfo) noexcept
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

void TypeInfo::SetTypeName(ContainerTraits::StringType&& name) { _typeName = std::move(name); }

void TypeInfo::SetTypeId(TypeId typeId) { _typeId = typeId; }

bool TypeInfo::CanConstruct() const { return (_constructionFn != nullptr); }

DataObject TypeInfo::Construct() const
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

bool TypeInfo::CanCopyConstruct() const { return (_copyConstructionFn != nullptr); }

DataObject TypeInfo::DangerousCopyConstruct(const void* src) const
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

bool TypeInfo::CanMoveConstruct() const { return (_moveConstructionFn != nullptr); }

DataObject TypeInfo::DangerousMoveConstruct(void* src) const
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

bool TypeInfo::IsLValueReference() const { return (_flags & Flags::IsLReference) != Flags::None; }

bool TypeInfo::IsRValueReference() const { return (_flags & Flags::IsRReference) != Flags::None; }

bool TypeInfo::IsReference() const { return (_flags & (Flags::IsLReference | Flags::IsRReference)) != Flags::None; }

void TypeInfo::SetIsConst() { _flags |= Flags::IsConst; }

void TypeInfo::SetIsPointer() { _flags |= Flags::IsPointer; }

void TypeInfo::SetIsLReference() { _flags |= Flags::IsLReference; }

void TypeInfo::SetIsRReference() { _flags |= Flags::IsRReference; }

} // namespace Entropy
