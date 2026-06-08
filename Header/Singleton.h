#pragma once

//주의: 생성자에서 다른 싱글턴 클래스 상호 참조시 데드락이 발생할 수 있음
//생성 과정은 반드시 단방향으로만 진행할것
#define DECLARE_SINGLETON(_class_) \
	public: \
		static _class_& GetInst() \
		{ \
			static _class_ inst; \
			return inst; \
		} \
 \
	private: \
		_class_(); \
		~_class_(); \
 \
		_class_(const _class_&) = delete; \
		_class_(_class_&&) = delete;