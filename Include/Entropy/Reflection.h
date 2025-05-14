// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/AttributeCollection.h"
#include "Entropy/Reflection/Details/ClassCounter.h"
#include "Entropy/Reflection/Details/MemberEnumeration.h"
#include "Entropy/Reflection/Details/ReflectionMacros.h"
#include "Entropy/Reflection/Details/ReflectionUtilityFunctions.h"

#ifdef ENTROPY_RUNTIME_REFLECTION_ENABLED
#include "Entropy/Reflection/DataObject/DataObjectFactory.h"
#include "Entropy/Reflection/TypeInfo/RuntimeReflectionMethods.h"
#include "Entropy/Reflection/TypeInfo/TypeInfo.h"

// These must be included after TypeInfo.h
#include "Entropy/Reflection/DataObject/DataObject.inl"
#include "Entropy/Reflection/Details/TypeId.inl"
#include "Entropy/Reflection/TypeInfo/TypeInfo.inl"
#include "Entropy/Reflection/TypeInfo/TypeInfoRef.inl"
#include "Entropy/Reflection/TypeInfoModules/ClassTypeInfo.inl"
#endif
