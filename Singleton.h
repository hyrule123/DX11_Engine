#pragma once

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