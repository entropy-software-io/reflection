// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

namespace Entropy
{

template <typename T>
inline bool DataObject::IsExactType() const
{
    if (ENTROPY_LIKELY(_container))
    {
        return _container->_typeInfo == ReflectTypeAndGetTypeInfo<T>();
    }
    return false;
}

template <typename T>
inline bool DataObject::CanCastTo() const
{
    if (ENTROPY_LIKELY(_container))
    {
        return _container->_typeInfo->CanCastTo(ReflectTypeAndGetTypeInfo<T>());
    }
    return false;
}

} // namespace Entropy
