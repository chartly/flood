/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Core/API.h"
#include "Core/String.h"

NAMESPACE_CORE_BEGIN

//-----------------------------------//

/**
 * Types can have metadata attached to them in the form of attributes.
 */

enum struct TypeAttributeKind : uint8
{
	String,
	Integer,
	KeyValue,
	Flags
};

//-----------------------------------//

/**
 * Provides types with a fast RTTI (Runtime Type Information) system that 
 * will be used for fast dynamic type checking, reflection and serialization.
 */

enum struct TypeKind : uint8
{
	Primitive,
	Composite,
	Enumeration,
};

struct ReflectionContext;
enum struct ReflectionWalkType : uint8;

typedef void (*ReflectionWalkFunction)(ReflectionContext*, ReflectionWalkType);

struct API_CORE Type
{
	Type();
	Type(TypeKind kind, const char* name, uint16 size);

	// Meta type of the type.
	TypeKind kind;

	// Name of the type.
	const char* name;

	// Size of the type.
	uint16 size;

	// Attributes of the type.
	//Array<TypeAttribute> attributes;

	// Custom walk function.
	ReflectionWalkFunction serialize;
};

typedef HashMap<Type*> TypeMap; // keyed by const char *

// Gets if this type represents a primitive type.
API_CORE bool ReflectionIsPrimitive(const Type*);

// Gets if this type represents a composite type.
API_CORE bool ReflectionIsComposite(const Type*);

// Gets if this type represents an enum type.
API_CORE bool ReflectionIsEnum(const Type*);

// Returns if this type is of the given type.
API_CORE bool ReflectionIsEqual(const Type*, const Type*);

//-----------------------------------//

// Keeps a map with the types in the database.
struct API_CORE ReflectionDatabase
{
	TypeMap types;
};

// Registers a new type in the reflection database.
API_CORE bool ReflectionDatabaseRegisterType(ReflectionDatabase*, Type* type);

// Gets the type for a given type.
#define ReflectionGetType(T) T##GetType()

// Gets a type given a name.
API_CORE Type* ReflectionFindType(const char*);

// Registers a new type.
API_CORE bool ReflectionRegisterType(Type*);

// Gets the reflection database.
API_CORE ReflectionDatabase& ReflectionGetDatabase();

//-----------------------------------//

struct Object;
struct Class;
struct Field;

typedef uint16 ClassId;
typedef uint8 FieldId;

typedef void* (*ClassCreateFunction)(Allocator*);

typedef HashMap<Field*> ClassFieldIdMap; // keyed by FieldId
typedef HashMap<Class*> ClassIdMap; // keyed by ClassId

/**
 * This class provides types with a fast RTTI (Runtime Type Information)
 * system that will be used for fast dynamic type checking and reflection.
 * Main use is for serialization and property introspection.
 */

struct API_CORE Class : public Type
{
	Class() : parent(nullptr), create_fn(nullptr)
	{ }

	// Class id.
	ClassId id;

	// Parent of the type.
	Class* parent;

	// Factory function.
	ClassCreateFunction create_fn;

	// Keeps track of the type fields.
	Array<Field*> fields;

	// Keeps track of the type fields by id.
	ClassFieldIdMap fieldIds;

	// Keeps track of the childs of the class.
	Array<Class*> childs;
};

// Returns the parent of the class.
#define ClassGetParent(klass) (klass->parent)

// Returns if this class has a parent.
#define ClassHasParent(klass) (ClassGetParent(klass) != nullptr)

// Returns if this type inherits from the given type.
API_CORE bool ClassInherits(const Class* klass, const Class* test);

// Returns the type from the given class instance at runtime.
API_CORE Class* ClassGetType(const Object*);

// Returns if the class is abstract.
API_CORE bool ClassIsAbstract(const Class*);

// Adds a new field to the given class.
API_CORE void ClassAddField(Class*, Field* field);

// Gets the field with the given name.
API_CORE Field* ClassGetField(const Class*, const char* name);

// Gets a field from a class hierarchy by its id.
API_CORE Field* ClassGetFieldById(Class* klass, FieldId id);

// Gets the field with the given name.
API_CORE void* ClassGetFieldAddress(const void*, const Field*);

// Creates a new instance of the class.
API_CORE void* ClassCreateInstance(const Class*, Allocator*);

// Calculates the class id of the given class.
API_CORE ClassId ClassCalculateId(const Class*);

// Gets the class id map.
API_CORE ClassIdMap& ClassGetIdMap();

// Gets the class by an id.
API_CORE Class* ClassGetById(ClassId id);

//-----------------------------------//

typedef void (*FieldSetterFunction)(void* object, void* value);
typedef void* (*FieldResizeFunction)(void* object, size_t size);

enum struct FieldQualifier : uint16
{
	Array           = 1 << 0,
	Map             = 1 << 1,
	Set             = 1 << 2,
	Handle          = 1 << 3,
	RawPointer      = 1 << 4,
	SharedPointer   = 1 << 5,
	RefPointer      = 1 << 6,
	ReadOnly        = 1 << 7,
	NoSerialize     = 1 << 8
};

struct API_CORE Field
{
	Field();

	Type* type;
	FieldId id;
	const char* name;
	Array<const char*> aliases;
	uint16 size;
	uint16 offset;
	uint16 pointer_size;
	FieldQualifier qualifiers;
	FieldSetterFunction setter;
	FieldResizeFunction resize;
	ReflectionWalkFunction serialize;
};

static FieldId FieldInvalid = 127;

// Returns if the field has the given qualifier.
API_CORE bool FieldHasQualifier(const Field*, FieldQualifier);

// Sets the given qualifier in the field.
API_CORE void FieldSetQualifier(Field*, FieldQualifier);

// Sets the given setter in the field.
API_CORE void FieldSetSetter(Field*, FieldSetterFunction);

#define FieldIsArray(f)         FieldHasQualifier(f, FieldQualifier::Array)
#define FieldIsHandle(f)        FieldHasQualifier(f, FieldQualifier::Handle)
#define FieldIsRawPointer(f)    FieldHasQualifier(f, FieldQualifier::RawPointer)
#define FieldIsRefPointer(f)    FieldHasQualifier(f, FieldQualifier::RefPointer)
#define FieldIsSharedPointer(f) FieldHasQualifier(f, FieldQualifier::SharedPointer)
#define FieldIsReadOnly(f)      FieldHasQualifier(f, FieldQualifier::ReadOnly)
#define FieldIsSerializable(f)  (!FieldHasQualifier(f, FieldQualifier::NoSerialize))
#define FieldIsPointer(f)       (FieldIsRawPointer(f) || FieldIsRefPointer(f) || FieldIsSharedPointer(f) || FieldIsHandle(f))

//-----------------------------------//

enum struct PrimitiveTypeKind : uint8
{
	Bool,
	Int8,
	Uint8,
	Int16,
	Uint16,
	Int32,
	Uint32,
	Int64,
	Uint64,
	Float,
	String,
	Color,
	Vector3,
	Quaternion,
};

struct API_CORE Primitive : public Type
{
	Primitive()
	{}

	Primitive(PrimitiveTypeKind kind, const char* name, uint16 size)
		: Type(TypeKind::Primitive, name, size), kind(kind)
	{}

	PrimitiveTypeKind kind;
};

struct API_CORE PrimitiveBuiltins
{
	PrimitiveBuiltins();

	Primitive p_bool;
	Primitive p_int8;
	Primitive p_uint8;
	Primitive p_int16;
	Primitive p_uint16;
	Primitive p_int32;
	Primitive p_uint32;
	Primitive p_int64;
	Primitive p_uint64;
	Primitive p_float;
	Primitive p_double;
	Primitive p_string;
	Primitive p_Vector3;
	Primitive p_Color;
	Primitive p_Quaternion;
};

// Gets the primitive builtin types.
API_CORE PrimitiveBuiltins& PrimitiveGetBuiltins();

//-----------------------------------//

typedef HashMap<int32> EnumValuesMap; // keyed by const char *
typedef HashMap<const char*> EnumNamesMap;

struct API_CORE Enum : public Type
{
	EnumValuesMap values;
	EnumNamesMap names;
	PrimitiveTypeKind backing;
};

// Adds a new enumeration to this enum.
API_CORE void EnumAddValue(Enum*, const char* name, int32 value);

// Gets the value of this enum name.
API_CORE int32 EnumGetValue(Enum*, const char* name);

// Gets the name of this enum value.
API_CORE const char* EnumGetValueName(Enum*, int32 value);

//-----------------------------------//

// Gets the value of the field in the object.
template<typename T>
const T& FieldGet( const Field* field, void* object )
{
	T* addr = (T*) ClassGetFieldAddress(object, field);
	return *addr;
}

// Sets the value of the field in the object.
template<typename T>
void FieldSet( const Field* field, void* object, const T& value )
{
	T* addr = (T*) ClassGetFieldAddress(object, field);
	FieldSetterFunction setter = field->setter;
	
	if(setter) setter(object, (void*) &value);
	else *addr = value;
}

// Recursively finds and creates instances of child classes.
template<typename T>
void ClassCreateChilds(const Class* klass, Allocator* alloc, Array<T*>& instances)
{
	for( size_t i = 0; i < klass->childs.size(); i++ )
	{
		Class* child = klass->childs[i];
		if( !child ) continue;

		ClassCreateChilds(child, alloc, instances);

		if( ClassIsAbstract(child) )
			continue;

		T* object = (T*) ClassCreateInstance(child, alloc);
		if(!object) continue;

		instances.pushBack(object);
	}
}

//-----------------------------------//

NAMESPACE_CORE_END

#include "Core/ReflectionHelpers.h"

#ifdef COMPILER_CLANG
#define fl_reflect(b) __attribute__((annotate("fl:reflect="#b)))
#define fl_attribute(...) __attribute__((annotate("fl:attributes="#__VA_ARGS__)))
#else
#define fl_reflect(b)
#define fl_attribute(...)
#endif