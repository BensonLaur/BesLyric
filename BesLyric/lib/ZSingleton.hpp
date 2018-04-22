#pragma once
#include <memory>


	/* ����ģʽ��ʵ�֣�һ��ģ�� */
	
	//�ϸ���˵����ģ�岢���������ĵ���ģʽ�������ĵ���ģʽ�����Ҫ���㣺 ���ɱ����������캯��˽�У������ɱ���ֵ����ֵ�͸��ƹ��캯��˽�У�
	//	��Σ��̳и�������� ���û�н��Լ��Ĺ��캯���͸�ֵ���� ��Ϊ˽�У������໹�ǿ��Ա����������͸�ֵ
	template<typename T>
	class ZSingleton
	{
	public:
		static std::auto_ptr<T> m_ptrInstance;		//ʵ������
		//static ZCS m_cs;							//�ٽ���		

	public:
		ZSingleton() {}
		virtual ~ZSingleton(){}

		//��ȡʵ��
		static T *GetInstance()
		{
			if (NULL == m_ptrInstance.get())
			{
				//m_cs.Enter();
				if (NULL == m_ptrInstance.get())
				{
					 m_ptrInstance = std::auto_ptr<T>(new T());  //����ָ������ʵ��
				}
				//m_cs.Leave();
			}

			return m_ptrInstance.get();
		}
	};

	//ʵ�����
	template<typename T>
	std::auto_ptr<T> ZSingleton<T>::m_ptrInstance;

	////�ٽ���
	//template<typename T>
	//ZCS ZSingleton<T>::m_cs;


	/*
		�ȽϹ淶�ĵ���ģʽ��ģ��ʵ�֣�

		template<typename T>
		class Singleton
		{
			public:
			inline static T& GetInstance()
			{
				static T s_instance;
				return s_instance;
			}

			private:
			Singleton(){}
			Singleton(Singleton const&): Singleton(){}
			Singleton& operator= (Singleton const&){return *this;}
			~Singleton(){}
		};

		//ʹ��ʾ����
		#include "ZSingleton.h"
		class Example
		{

		};
		typedef Singleton<Example> SglExample;
		#define SGL_EXAMPLE SglExample::GetInstance()
	
		//��ʵ�ֵ���ȱ�㣺
		�ŵ㣺�߼��Ϸ����˵���ģʽ�����
		ȱ�㣺����û��ʲô��չ�ԣ����� ��ģ����ӳ�Ա ������ά��
	*/


	/* ����ģʽ��ʵ�֣������궨�� */

	//������Ĭ�Ϲ��캯���������ʹ��
#define SINGLETON_0(_CLASS_)								\
public:														\
	inline static _CLASS_* GetInstance()					\
	{														\
		static _CLASS_ s_instance;							\
		return &s_instance;									\
	}														\
private:													\
	_CLASS_();												\
	_CLASS_(_CLASS_ const&){}					\
	_CLASS_& operator= (_CLASS_ const&) { return *this; }	\
	~_CLASS_();

	//������1��û��Ĭ�Ϲ��캯���������ʹ��
#define SINGLETON_1(_CLASS_, _BASE_CLASS_, PARAMETER)						\
public:																		\
	inline static _CLASS_* GetInstance()									\
	{																		\
		static _CLASS_ s_instance;											\
		return &s_instance;													\
	}																		\
private:																	\
	_CLASS_();																\
	_CLASS_(_CLASS_ const&) : _BASE_CLASS_(PARAMETER) {}					\
	_CLASS_& operator= (_CLASS_ const&) { return *this; }					\
	~_CLASS_();

	//������2��û��Ĭ�Ϲ��캯���������ʹ��
#define SINGLETON_2(_CLASS_, _BASE_CLASS_1, PARAMETER_1,_BASE_CLASS_2, PARAMETER_2)					\
public:																								\
	inline static _CLASS_* GetInstance()															\
	{																								\
		static _CLASS_ s_instance;																	\
		return &s_instance;																			\
	}																								\
private:																							\
	_CLASS_();																						\
	_CLASS_(_CLASS_ const&) :_BASE_CLASS_1(PARAMETER_1),_BASE_CLASS_2(PARAMETER_2) {}				\
	_CLASS_& operator= (_CLASS_ const&) { return *this; }											\
	~_CLASS_();


	//������Ĺ���������������������캯�����������������Լ�ʵ�֣�����ֱ���ں����ⶨ��ú꣩  
#define SINGLETON_C_D_0(_CLASS_)			\
	_CLASS_::_CLASS_() {}					\
	_CLASS_::~_CLASS_() {}    

#define SINGLETON_C_D_1(_CLASS_, _BASE_CLASS_, PARAMETER)			\
	_CLASS_::_CLASS_():_BASE_CLASS_(PARAMETER) {}					\
	_CLASS_::~_CLASS_() {}    

#define SINGLETON_C_D_3(_CLASS_, _BASE_CLASS_1, PARAMETER_1,_BASE_CLASS_2, PARAMETER_2)			\
	_CLASS_::_CLASS_():_BASE_CLASS_1(PARAMETER_1),_BASE_CLASS_2(PARAMETER_2) {}					\
	_CLASS_::~_CLASS_() {}    

	/*

	//ʹ��ʾ����
	#include "ZSingleton.h"  
  
	class Example  
	{  
		SINGLETON_0(Example);  
	};  
	#define SGL_EXAMPLE Example::GetInstance() 

	//.cpp �ļ���ʹ��
	SINGLETON_C_D_0(Example)


	//��ʵ�ֵ���ȱ�㣺
	�ŵ㣺�߼��Ϸ����˵���ģʽ����ƣ��Ҿ��кܺõ���չ��
	ȱ�㣺ʵ��ȷʵ�е��

	*/

