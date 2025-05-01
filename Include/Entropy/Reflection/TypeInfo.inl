// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

namespace Entropy
{

void TypeInfo::SetTypeName(ContainerTraits::StringType&& name) { _typeName = std::move(name); }

bool TypeInfo::CanConstruct() const { return (_constructionFn != nullptr); }

DataObject TypeInfo::Construct() const
{
    if (ENTROPY_LIKELY(CanConstruct()))
    {
        void* data = _constructionFn();
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(this, data);
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
            return DataObject(this, data);
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
            return DataObject(this, data);
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

} // namespace Entropy
