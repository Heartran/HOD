#pragma once
#include<iostream>
#include<string>
#include<boost/utility/enable_if.hpp>
#include<vector>
#include<algorithm>
#include<assert.h>
#include<memory>
#include<type_traits>

namespace ContainerHelper{

//T�̃X�[�p�[�N���X�������Ă���ꍇ��肭�����Ȃ�
template<typename T, typename RETURN_TYPE >
class has_getName {
protected:
	template<typename U, RETURN_TYPE(U::*)(void)const >
	struct aux {};

	typedef char yes;
	typedef struct { char foo[2]; } no;

	template<typename U>
	static yes test(U*, aux<U, &U::getName>* = 0);
	template<typename U>
	static no test(...);

public:
	static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
};

template< typename T >
class has_getName_function_return_type_is_const_char_ptr : public has_getName<T, const char*> {};

template< typename T >
class has_getName_function_return_type_is_const_string_ref : public has_getName<T, const std::string&> {};

class name_is
{
public:
	name_is(const char* name):target_(name){}

	//getName()��const char*��Ԃ��ꍇ
	//std::vector<T*>�̏ꍇ
	template<typename T >
	bool operator()(T *obj, typename boost::enable_if< has_getName_function_return_type_is_const_char_ptr<T> >::type* = 0 ) const
	{
		return strcmp(obj->getName(), target_) == 0;
	}
	
	//getName()��const char*��Ԃ��ꍇ
	//T[]�̏ꍇ
	template<typename T>
	bool operator()(const T &obj,  typename boost::enable_if< has_getName_function_return_type_is_const_char_ptr<T> >::type* = 0 ) const
	{
		return strcmp(obj.getName(), target_) == 0;
	}

	//getName()��const std::string&��Ԃ��ꍇ
	//std::vector<T*>�̏ꍇ
	template<typename T >
	bool operator()(T *obj, typename boost::enable_if< has_getName_function_return_type_is_const_string_ref<T> >::type* = 0 ) const
	{
		return obj->getName()==target_;
	}

	//getName()��const std::string&��Ԃ��ꍇ
	//std::vector<std::shared_ptr<T>>�̏ꍇ
	template<typename T >
	bool operator()(std::shared_ptr<T> obj, typename boost::enable_if< has_getName_function_return_type_is_const_string_ref<T> >::type* = 0 ) const
	{
		return obj->getName()==target_;
	}

	//getName()��const std::string&��Ԃ��ꍇ
	//std::vector<std::weak_ptr<T>>�̏ꍇ
	template<typename T >
	bool operator()(std::weak_ptr<T> obj, typename boost::enable_if< has_getName_function_return_type_is_const_string_ref<T> >::type* = 0 ) const
	{
		if( std::shared_ptr<T> p = obj.lock() ){
			return p->getName()==target_;
		}
		return false;
	}



	//getName()��const std::string&��Ԃ��ꍇ
	//T t[]�̏ꍇ
	template<typename T>
	bool operator()(const T &obj,  typename boost::enable_if< has_getName_function_return_type_is_const_string_ref<T> >::type* = 0 ) const
	{
		return obj.getName()==target_;
	}
private:
	const char* target_;
};

//�X�[�p�[�N���X��has_getName����`���Ă���ꍇ
class name_is2
{
public:
	name_is2(const char* name):target_(name){}

	//getName()��const char*��Ԃ��ꍇ
	//std::vector<T*>�̏ꍇ
	template<typename T >
	bool operator()(T *obj, typename boost::enable_if< has_getName_function_return_type_is_const_char_ptr< typename T::BaseType > >::type* = 0 ) const
	{
		return strcmp(obj->getName(), target_) == 0;
	}

	//getName()��const char*��Ԃ��ꍇ
	//std::vector< std::shared_ptr<T> >�̏ꍇ
	template<typename T >
	bool operator()( const std::shared_ptr<T> &obj, typename boost::enable_if< has_getName_function_return_type_is_const_char_ptr< typename T::BaseType > >::type* = 0 ) const
	{
		return strcmp(obj->getName(), target_) == 0;
	}
	
	//getName()��const char*��Ԃ��ꍇ
	//T[]�̏ꍇ
	template<typename T>
	bool operator()(const T &obj,  typename boost::enable_if< has_getName_function_return_type_is_const_char_ptr< typename T::BaseType > >::type* = 0 ) const
	{
		return strcmp(obj.getName(), target_) == 0;
	}

	//getName()��const std::string&��Ԃ��ꍇ
	//std::vector<T*>�̏ꍇ
	template<typename T >
	bool operator()(T *obj, typename boost::enable_if< has_getName_function_return_type_is_const_string_ref< typename T::BaseType > >::type* = 0 ) const
	{
		return obj->getName()==target_;
	}

	//getName()��const std::string&��Ԃ��ꍇ
	//std::vector< std::shared_ptr<T> >�̏ꍇ
	template<typename T >
	bool operator()( const std::shared_ptr<T> &obj, typename boost::enable_if< has_getName_function_return_type_is_const_string_ref<typename T::BaseType > >::type* = 0 ) const
	{
		return obj->getName()==target_;
	}

	//getName()��const std::string&��Ԃ��ꍇ
	//T t[]�̏ꍇ
	template<typename T>
	bool operator()(const T &obj,  typename boost::enable_if< has_getName_function_return_type_is_const_string_ref< typename T::BaseType > >::type* = 0 ) const
	{
		return obj.getName()==target_;
	}

private:
	const char* target_;
};

class findByName
{	
public:
	findByName(const char* name):target_(name){}

	template <typename T>
	typename std::vector<T*>::iterator operator()(std::vector<T*> &source) const
	{
		static_assert(
				has_getName_function_return_type_is_const_string_ref<T>::value |
				has_getName_function_return_type_is_const_char_ptr<T>::value ,
				"getName()const member function is not found." );

		return std::find_if(source.begin(), source.end(), name_is(target_));
	}

	//dynamic array�p
	template <typename T>
	typename T* operator()( T* source, unsigned long size ) const
	{
			static_assert(
				has_getName_function_return_type_is_const_string_ref<T>::value |
				has_getName_function_return_type_is_const_char_ptr<T>::value ,
				"getName()const member function is not found." );

		T* t = std::find_if( source, source+size, name_is(target_));
		if( t != source+size ){
			return t;
		} else {
			return NULL;
		}
	}
	//shared_ptr�p
	template <typename T>
	typename std::vector< std::shared_ptr<T> >::iterator operator()(std::vector< std::shared_ptr<T> > &source) const
	{
		static_assert(
				has_getName_function_return_type_is_const_string_ref<T>::value |
				has_getName_function_return_type_is_const_char_ptr<T>::value ,
				"getName()const member function is not found." );

		return std::find_if(source.begin(), source.end(), name_is(target_));
	}

	//weak_ptr�p
	template <typename T>
	typename std::vector< std::weak_ptr<T> >::iterator operator()(std::vector< std::weak_ptr<T> > &source) const
	{
		static_assert(
				has_getName_function_return_type_is_const_string_ref<T>::value |
				has_getName_function_return_type_is_const_char_ptr<T>::value ,
				"getName()const member function is not found." );

		return std::find_if(source.begin(), source.end(), name_is(target_));
	}
private:
	const char* target_;	
};

//�X�[�p�[�N���X��getName�������Ă���Ə�肭�����Ȃ��̂ł�����Ɩ�����
//���̃N���X��
//	typedef "BaseClassName" BaseType;
//�Ɛ錾���邱��
class findByName2
{	
public:
	findByName2(const char* name):target_(name){}

	template <typename T>
	typename std::vector<T*>::iterator operator()(std::vector<T*> &source) const
	{
		static_assert(
			std::is_base_of< typename T::BaseType, T >::value,
			" declare BaseType." 
		);

		static_assert(
				has_getName_function_return_type_is_const_string_ref<typename T::BaseType>::value |
				has_getName_function_return_type_is_const_char_ptr<typename T::BaseType>::value,
				"getName()const member function is not found." );

		return std::find_if(source.begin(), source.end(), name_is2(target_));
	}

	//dynamic array�p
	template <typename T>
	typename T* operator()( T* source, unsigned long size ) const
	{
		static_assert(
			std::is_base_of< typename T::BaseType, T >::value,
			" declare BaseType." 
		);
		
		static_assert(
			has_getName_function_return_type_is_const_string_ref<typename T::BaseType>::value |
			has_getName_function_return_type_is_const_char_ptr<typename T::BaseType>::value,
			"getName()const member function is not found." );

		T* t = std::find_if( source, source+size, name_is2(target_));
		if( t != source+size ){
			return t;
		} else {
			return NULL;
		}
	}
	//shared_ptr�p
	template <typename T>
	typename std::vector< std::shared_ptr<T> >::iterator operator()(std::vector< std::shared_ptr<T> > &source) const
	{
		static_assert(
			std::is_base_of< typename T::BaseType, T >::value,
			" declare BaseType." 
		);

		static_assert(
				has_getName_function_return_type_is_const_string_ref<typename T::BaseType>::value |
				has_getName_function_return_type_is_const_char_ptr<typename T::BaseType>::value,
				"getName()const member function is not found." );

		return std::find_if(source.begin(), source.end(), name_is2(target_));
	}
private:
	const char* target_;	
};

//�g�������o�[�֐�
template <typename T>
typename std::vector<T>::iterator operator|(std::vector<T> &source, findByName f)
{
	return f(source);
}

//�g�������o�[�֐�
template <typename T>
typename std::vector<T>::iterator operator|(std::vector<T> &source, findByName2 f)
{
	return f(source);
}

//helper�֐�
template <typename T>
typename std::vector<T>::iterator findByNameInVector(std::vector<T> &source, const char* pName )
{
	findByName func = findByName(pName);
	return func(source);
}

//helper�֐�
template <typename T>
typename T* findByNameInArray( T* source, unsigned long size, const char* pName )
{
	findByName func = findByName(pName);
	return func( source, size );
}

template<typename T, int N>
typename T* findByNameInArray(  T (&a)[N], const char* pName )
{
	findByName func = findByName(pName);
	return func( a, N );
}

}
