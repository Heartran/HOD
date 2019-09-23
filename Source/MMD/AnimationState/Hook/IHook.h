#ifndef __MMD_ANIMATIONSTATE_HOOK_IHOOK_H__
#define __MMD_ANIMATIONSTATE_HOOK_IHOOK_H__

#include<memory>


//Listener, Hook, Observer�Ƃ����O���������ǁA
//informUpdate�Ƃ�����
//�X�V���t�b�N����݂����Ȍ������̂ق���������₷���Ǝv�����B


namespace MMD{
namespace AnimationState{
class BaseAnimationState;

namespace Hook {
	class IHook;
	typedef std::shared_ptr<IHook> IHookSharedPtr;
class IHook {
	public:
		IHook(){}
		virtual ~IHook(){}
		virtual void hookUpdate( float currentFrame )=0;
		virtual void hookStart()=0;
		virtual void hookChangeState()=0;
		virtual void hookAnimationOver()=0;
	private:
		IHook( const IHook & );
		void operator=( const IHook & );
};

}//MMD
}//AnimationState
}//Hook

#endif

