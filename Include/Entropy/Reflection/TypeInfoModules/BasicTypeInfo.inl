// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

// Code normally in a cpp file is inlined because member types can be changed via type traits by the user.

namespace Entropy
{
namespace Reflection
{

inline bool BasicTypeInfo::CanConstruct() const { return (_constructionFn != nullptr); }

inline DataObject BasicTypeInfo::Construct() const
{
    if (ENTROPY_LIKELY(CanConstruct()))
    {
        void* data = _constructionFn();
        if (ENTROPY_LIKELY(data))
        {
            return DataObject(_typeInfo, data);
        }
    }
    return nullptr;
}

inline void BasicTypeInfo::Destruct(void* dataPtr) const
{
    if (ENTROPY_LIKELY(_destructionFn != nullptr))
    {
        _destructionFn(dataPtr);
    }
}

} // namespace Reflection
} // namespace Entropy
